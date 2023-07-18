#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include <memory>
#include<utility>
#include<unordered_map>
#include <cmath>

#include "doctest.h"
#include "yaml-cpp/yaml.h"

#include "Cavity.h"
#include "ActiveCavity.h"
#include "PassiveCavity.h"
#include "FileParser.h"

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

bool TestDouble(std::unordered_map<std::string,std::string> map, std::string key){
  if(map.count(key)==0){
    std::cerr << "Couldn't find " << key <<  std::endl;
    return false;
  }
  else{
    try{
      double temp = std::stod(map.at(key));          
    }
    catch(...){
      std::cout << "Couldn't convert " << map.at(key) << " to a double" << std::endl;
      return false;
    }
  return true;
  }
}

TEST_CASE("Testing TestDouble...") {
    double burner;
    std::unordered_map<std::string,std::string> test_map;
    // Key that can be converted
    test_map["one"] = "112.234";
    // Key that can't be converted
    test_map["three"] = "doesn't_work"; 
    // test_map["two"] doesn't exist
    CHECK(TestDouble(test_map,"one" ) == true);
    CHECK(TestDouble(test_map,"two" ) == false);
    CHECK(TestDouble(test_map,"three" ) == false);
}

bool ValidityChecking(std::unordered_map<std::string,std::string> map, std::string key, ValidityCheckingFlags flag, double boundary1, double boundary2){
  bool output = true;
  if(TestDouble(map,key)){
    double val = std::stod(map.at(key));
    switch (flag){
        case IS_VALID:
            break;
        case MIN_EXCLUSIVE:
            if(val <= boundary1){
              std::cerr << key << " must be strictly greater than " << boundary1 << std::endl;
              output= false;
            }
            break;
        case MIN_INCLUSIVE:
            if(val < boundary1){
              std::cerr << key << " must be greater than or equal to " << boundary1 << std::endl;
              output= false;
            }
            break;
        case MAX_EXCLUSIVE:
            if(val >= boundary1){
              std::cerr << key << " must be strictly less than " << boundary1 << std::endl;
              output = false;
            }
            break;
        case MAX_INCLUSIVE:
            if(val > boundary1){
              std::cerr << key << " must be less than or equal to " << boundary1 << std::endl;
              output = false;
            }
            break;
        case MIN_EXCLUSIVE_MAX_EXCLUSIVE:
            if((val <= boundary1) || (val >= boundary2) ){
              std::cerr << key << " must be strictly greater than " << boundary1 << " and strictly less than " << boundary2 <<  std::endl;
              output = false;
            }
            break;
        case MIN_INCLUSIVE_MAX_INCLUSIVE:
            if((val < boundary1) || (val > boundary2) ){
              std::cerr << key << " must be greater than or equal to" << boundary1 << " and less than or equal to " << boundary2 <<  std::endl;
              output =  false;
            }
            break;
        case MIN_EXCLUSIVE_MAX_INCLUSIVE:
            if((val <= boundary1) || (val > boundary2) ){
              std::cerr << key << " must be strictly greater than " << boundary1 << " and less than or equal to " << boundary2 <<  std::endl;
              output = false;
            }
            break;
        case MIN_INCLUSIVE_MAX_EXCLUSIVE:
            if((val < boundary1) || (val >= boundary2) ){
              std::cerr << key << " must be greater than or equal to" << boundary1 << " and strictly less than " << boundary2 <<  std::endl;
              output = false;
            }
            break;
        default:
// we got an invalid flag. we return false
            output = false;
    }
  }
  if(!output){
    std::cerr << "Couldn't read " << key <<  std::endl;
  }
  return output;
}

bool ValidateInputs(std::unordered_map<std::string,std::string> InputFileMap){
// Check that nturns exists and is greater than or equal to 1
   bool output =  !(ValidityChecking(InputFileMap, "nturns", MIN_INCLUSIVE, 1) &&
// Check that npop exists and is strictly greater than or equal to 1
                    ValidityChecking(InputFileMap, "npop", MIN_INCLUSIVE, 1) &&
// Check that nbunches exists and is strictly greater than or equal to 1
                    ValidityChecking(InputFileMap, "nbunches", MIN_INCLUSIVE, 1) &&
// Check that Vrf exists and is greater than or equal to 0
                    ValidityChecking(InputFileMap, "Vrf", MIN_INCLUSIVE, 0) &&
// Check that nharm exists and is greater or equal to 1
                    ValidityChecking(InputFileMap, "nharm", MIN_INCLUSIVE, 1) &&
// Check that frf exists and is greater or or equal to 0
                    ValidityChecking(InputFileMap, "frf", MIN_INCLUSIVE, 0) &&
// Check that E0 exists and is strictly greater than 0
                    ValidityChecking(InputFileMap, "E0", MIN_EXCLUSIVE, 0) &&
// Check that sig_d exists and is strictly greater than 0
                    ValidityChecking(InputFileMap, "sig_d", MIN_EXCLUSIVE, 0)

                   );
    return output;
}

bool ReadInputParameters(std::string fname, std::unordered_map<std::string,std::string>& InputFileMap){
// Read .yaml file for input parameters. Converts all values to doubles. 
// Validates that parameters behave as expected. R
// Returns false if you can't convert everything to doubles
  YAML::Node config;
  try{
    config = YAML::LoadFile(fname);
  }
  catch(...){
    std::cerr << "Couldn't parse config file: " << fname <<  std::endl;
    return false;
  }
// Convert YAML node structure to unordered map of string to doubles
  for (YAML::const_iterator it=config.begin();it!=config.end();++it) {
    std::string key = it->first.as<std::string>();
    std::string str_val = it->second.as<std::string>();
    double value;
    if(!StrToDouble(str_val,value)){
        std::cout << "Attempted to convert: " << str_val << " to C++ style double " << std::endl;
        return false;
    }
    InputFileMap.insert({key,str_val});
  }
  return ValidateInputs(InputFileMap);
}

bool ReadCavityParameters(std::string fname,const std::unordered_map<std::string,std::string> InputParameterMap,std::vector<std::unique_ptr<Cavity>>& cavities){
// Read in contents of .yaml file containing cavity data
  YAML::Node config;
  try{
    config = YAML::LoadFile(fname);
  }
  catch(...){
    std::cerr << "Couldn't parse config file: " << fname <<  std::endl;
    return false;
  }
    
  YAML::Node cavData;
  try{
    cavData= config["cavities"];
  }
  catch(...){
    std::cerr << "Couldn't parse config file: " << fname <<  std::endl;
    return false;  
  }

// Interate through each cavity
  int count = 0;
  for ( YAML::const_iterator it = cavData.begin(); it != cavData.end(); ++it) {
    count++;
// Read in attributes of a particular cavity
    const YAML::Node& cav = *it;
    std::unordered_map<std::string,std::string> AttrMap;
    for(YAML::const_iterator attr = cav.begin(); attr != cav.end(); ++attr){
// Read in attributes of a cavity as string
      std::string key = attr->first.as<std::string>();
      std::string str_val = attr->second.as<std::string>();
      AttrMap.insert({key,str_val});
    }
// Make sure that a cavity actually has attributes
    if(AttrMap.empty()){
      return false;
    }
// Check that type, order, phase and either r or shunt exist
// Also checks if they are valid parameters
  std::string cname;
  std::string type;    
// determining name of cavity
    try{
      cname = AttrMap.at("name");
    }
    catch(...){
      std::cerr << "Couldn't find name of cavity number " << count << std::endl;
        return false;
    }
// determine active passive state of cavity
    try{
        type = AttrMap.at("type");
    }
    catch(...){
      std::cerr << "Couldn't find type of " << cname << std::endl;
        return false;    
    }
    if(AttrMap.at("type")=="active"){
// We check that r is a valid double, order is at least 1, and phase is between -180 and 180
        if(ValidityChecking(AttrMap, "r", IS_VALID) &&
          ValidityChecking(AttrMap, "order", MIN_INCLUSIVE, 1) &&
          ValidityChecking(AttrMap, "Phase", MIN_INCLUSIVE_MAX_INCLUSIVE, -180,180)){
            std::string cname = AttrMap.at("name");
            double r = std::stod(AttrMap.at("r"));
            double Phase = std::stod(AttrMap.at("Phase"));
            double Order = std::stod(AttrMap.at("order"));
            cavities.push_back(std::make_unique<ActiveCavity>(cname,r, Phase,Order));
        }
        else{
          return false;
        }
      }
      else if(AttrMap.at("type")=="passive"){
// We check that shunt is a valid double, order is at least 1, and phase is between -180 and 180
        if(ValidityChecking(AttrMap, "shunt", IS_VALID) &&
          ValidityChecking(AttrMap, "order", MIN_INCLUSIVE, 1) &&
          ValidityChecking(AttrMap, "Phase", MIN_INCLUSIVE_MAX_INCLUSIVE, -180,180)){
            std::string cname = AttrMap.at("name");
            double shunt = std::stod(AttrMap.at("shunt"));
            double Phase = std::stod(AttrMap.at("Phase"));
            double Order = std::stod(AttrMap.at("order"));
            cavities.push_back(std::make_unique<PassiveCavity>(cname,shunt, Phase, Order));
        }
        else{
          std::cerr << "Invalid cavity type" << std::endl;
          return false;
        }      
      }
      else{
        std::cerr << "Invalid type for cavity " << AttrMap.at("name") << std::endl;
        return false;
      }
  } // done iterating through all cavities. We check if any cavities were present
  if(cavities.empty()){
    return false;
  }
  return true;
}

void PrintInputMap(const std::unordered_map<std::string,std::string> Map){
    // Loop through all keys in map and print
    int counter = 0;
    for(auto& pair: Map){
      counter++;
      std::cout << pair.first << '\t' << pair.second << std::endl;
    }
}

bool OpenOutputFiles(std::vector<std::string> FileNames,std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMapping){
// We open a bunch of filestreams
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
// Write a string to a particular file
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
