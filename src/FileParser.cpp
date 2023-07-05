#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include <memory>
#include<utility>
#include<unordered_map>

bool StrToDouble(std::string parameter_value_str,double &out_value){
    // Convert Fortran double to C++ double
    if(parameter_value_str.empty()){
      return false;
    }
    std::string D = "d";
    size_t start_pos = parameter_value_str.find(D);
    if(start_pos == std::string::npos){
        try{
          out_value = std::stod(parameter_value_str);
        }
        catch(...){
            return false;
        }
    }
    else{
      std::string replacement = "E";
      parameter_value_str.replace(start_pos, D.length(), replacement);
        try{
          out_value = std::stod(parameter_value_str);
        }
        catch(...){
            return false;
        }
    }
    return true;    
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
      s>>parameter_name;
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