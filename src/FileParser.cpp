#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include <memory>
#include<utility>
#include<unordered_map>

#include "doctest.h"

bool StrToDouble(std::string parameter_value_str,double &out_value){
    // Convert Fortran double to C++ double
    // Empty string check
    if(parameter_value_str.empty()){
      return false;
    }
    // search for 'd' in string
    std::string d = "d";
    size_t start_pos = parameter_value_str.find(d);
    // if d not found, we loop through string. The only valid characters are digits, periods and negative signs
    int location = -1;
    if(start_pos == std::string::npos){
        bool check_for_period = true;
        for(auto character : parameter_value_str){
            location++;
          // Allow periods, but only once
          if(check_for_period && character=='.'){
            check_for_period = false;
            continue;
          }
          // All other characters must be valid digits
          else if(isdigit(character)){
            continue;
          }
          else if(character=='-'){
            // negative sign is the first character, indicating negative number.
            if(location==0){
                continue;            
            }
            // For this case, a negative sign should not appear anywhere else
            else{
              return false;
            }
          }
          else{
            return false;
          }
        }
        // try-catch for just in case the above misses something
        try{
          out_value = std::stod(parameter_value_str);
        }
        catch(...){
            return false;
        }
    }
    else{
      std::string replacement = "E";
      parameter_value_str.replace(start_pos, d.length(), replacement);
      bool check_for_period = true;
      bool check_for_E_flag = true;
      int E_loc = -1;
      int current_loc = -1;
      for(auto character : parameter_value_str){
        current_loc++;
      // Allow periods, but only once
        if(check_for_period && character=='.'){
          check_for_period = false;
          continue;
        }
      // check for E, but only once, and note location of E
        else if(check_for_E_flag && character=='E'){
          check_for_E_flag = false;
          E_loc = current_loc;
          continue;
        }
        else if(character=='-'){
          // If the negative sign appears first, or directly after E, then we good
          if(current_loc==(E_loc+1)){
            continue;
          }
          else{
            return false;        
          }
        }
      // All other characters must be valid digits
        else if(isdigit(character)){
          continue;
        }
        else{
          return false;
        }
      }
    // try-catch for just in case the above misses something
      try{
        out_value = std::stod(parameter_value_str);
      }
      catch(...){
          return false;
      }
    }
    return true;    
}

TEST_CASE("Testing StrToDouble...") {
    double burner;
    CHECK(StrToDouble("",burner) == false);
    CHECK(StrToDouble("123.23234234",burner) == true);    
    CHECK(StrToDouble("not_a_double",burner) == false);    
    CHECK(StrToDouble("3d9",burner) == true);
    CHECK(StrToDouble("3d9saf",burner) == false);
    CHECK(StrToDouble("-1123.345346",burner) == true);
    CHECK(StrToDouble("2.4d9",burner) == true);
    CHECK(StrToDouble("-2.4d9",burner) == true);
    CHECK(StrToDouble("-2.4d-9",burner) == true);
    CHECK(StrToDouble("-2.4d--9",burner) == false);
}


bool ReadContents(std::string fname,std::unordered_map<std::string,double>& ParameterMapping){
// Read in contents of .in file. Converts all values to doubles. Fails if you can't do this
// Create buffer
  std::string line;
  std::ifstream file (fname.c_str());
  if (file.is_open()){
    while( getline(file,line)){
    // Extract line and push to stringstream for processing
      std::stringstream s(line);
    // Extract first and second words in line
      std::string parameter_value_str, parameter_name;
      double parameter_value;
      s>>parameter_value_str;
    // If we read in a blank line, skip the line
      if(parameter_value_str.empty()){
        continue;
      }
      s>>parameter_name;
    // If no parameter name is given for a value, return false
      if(parameter_name.empty()){
        std::cout << "No name given to parameter " << parameter_value_str << std::endl;
          return false;
      }
    // Make sure that the parameter value can be converted to a double
      if(!StrToDouble(parameter_value_str,parameter_value)){
        file.close();
        std::cout << "Couldn't parse value of parameter " << parameter_name << std::endl;
        return false;
      }
      ParameterMapping.insert({parameter_name,parameter_value});
    }
    file.close();
  }
  else{
    std::cout << "Couldn't open file" << std::endl;
    return false;
  }
  return true;
}

void PrintMap(const std::unordered_map<std::string,double> Map){
    // Loop through all keys in map and print
    int counter = 0;
    for(auto& pair: Map){
      counter++;
      std::cout << pair.first << '\t' << pair.second << std::endl;
    }
}

bool OpenOutputFiles(std::vector<std::string> FileNames,std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMapping){
    for(std::string fname: FileNames){
        std::shared_ptr<std::ofstream> file = std::make_shared<std::ofstream>(fname);
        FileMapping[fname] = file;
        if(!FileMapping[fname] || !*FileMapping[fname]){
          return false;
        }
      }
    return true;
}

bool WriteToOutputFile(std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMap, std::string fname, std::string text){
    auto member = FileMap.find(fname);
    if (member != FileMap.end()) {
      auto file_stream = member->second;
      if(file_stream->is_open()){
        *file_stream << text << std::endl;
      }
      else{
        std::cerr << "File is not open" << std::endl;
        return false;
      }
      return true;
    }
    else {
      std::cerr << "Key not found" << std::endl;
      return false;
    }
  return true;
}