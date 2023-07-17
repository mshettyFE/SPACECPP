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

bool AboveMinExclusive(std::unordered_map<std::string,std::string> map, std::string key, double min_val){
  if(TestDouble(map,key)){
    double val = std::stod(map.at(key));
    if(val <= min_val){
      std::cerr << key << " must be strictly greater than " << min_val << std::endl;
      return false;
    }
  }
  else{
    return false;
  }
  return true;
}

bool AboveMinInclusive(std::unordered_map<std::string,std::string> map, std::string key, double min_val){
  if(TestDouble(map,key)){
    double val = std::stod(map.at(key));
    if(val < min_val){
      std::cerr << key << " must be greater than or equal to " << min_val << std::endl;
      return false;
    }
  }
  else{
    return false;
  }
  return true;
}

bool BelowMaxExclusive(std::unordered_map<std::string,std::string> map, std::string key, double max_val){
  if(TestDouble(map,key)){
    double val = std::stod(map.at(key));
    if(val <= max_val){
      std::cerr << key << " must be strictly less than " << max_val << std::endl;
      return false;
    }
  }
  else{
    return false;
  }
  return true;
}

bool BelowMaxInclusive(std::unordered_map<std::string,std::string> map, std::string key, double max_val){
  if(TestDouble(map,key)){
    double val = std::stod(map.at(key));
    if(val < max_val){
      std::cerr << key << " must be less than or equal to " << max_val << std::endl;
      return false;
    }
  }
  else{
    return false;
  }
  return true;
}

bool SandwichedBetweenInclusive(std::unordered_map<std::string,std::string> map, std::string key, double min_val,double max_val){
  if(max_val < min_val){
    std::cerr << "max_val must be strictly larger than min_val" << std::endl;
    return false;
  }
  if(TestDouble(map,key)){
    double val = std::stod(map.at(key));
    if((val < min_val) || (val > max_val)){
      std::cerr << key << " must be greater than or equal to " << min_val << " and less than or equal to " << max_val << std::endl;
      return false;
    }
  }
  else{
    return false;
  }
  return true;
}

bool SandwichedBetweenExclusive(std::unordered_map<std::string,std::string> map, std::string key, double min_val,double max_val){
  if(max_val <= min_val){
    std::cerr << "max_val must be strictly larger than min_val" << std::endl;
    return false;
  }
  if(TestDouble(map,key)){
    double val = std::stod(map.at(key));
    if((val <= min_val) || (val >= max_val)){
      std::cerr << key << " must be strictly greater than " << min_val << " and strictly less than " << max_val << std::endl;
      return false;
    }
  }
  else{
    return false;
  }
  return true;
}

bool ValidateInputs(std::unordered_map<std::string,std::string> InputFileMap){
// Check that nturns exists and is strictly greater than 0
  std::string var;
  var = "nturns";
  if(!AboveMinExclusive(InputFileMap, var, 0)){
    std::cerr << "Couldn't read " << var <<  std::endl;
    return false;
  }
// Check that npop exists and is strictly greater than 1
  var = "npop";
  if(!AboveMinExclusive(InputFileMap, var, 1)){
    std::cerr << "Couldn't read " << var <<  std::endl;
    return false;
  }
// Check that nbunches exists and is strictly greater than 1
  var = "nbunches";
  if(!AboveMinExclusive(InputFileMap, var, 1)){
    std::cerr << "Couldn't read " << var <<  std::endl;
    return false;
  }
// Checks that Vrf exists and is strictly greater than 0
  var = "Vrf";
  if(!AboveMinExclusive(InputFileMap, var, 0)){
    std::cerr << "Couldn't read " << var <<  std::endl;
    return false;
  }
// check that nharm is strictly greater than 1
  var = "nharm";
  if(!AboveMinExclusive(InputFileMap,var,1)){
    std::cerr << "Couldn't read " << var <<  std::endl;
    return false;
  }

// check that frf is strictly greater than 0
  var = "frf";
  if(!AboveMinExclusive(InputFileMap,var,0)){
    std::cerr << "Couldn't read " << var <<  std::endl;
    return false;
  }

// Check that E0 exists and is strictly positive
  var = "E0";
  if(!AboveMinExclusive(InputFileMap, var, 0)){
    std::cerr << "Couldn't read " << var <<  std::endl;
    return false;
  }
    
// Check that sig_d is strictly positive
  var = "sig_d";
  if(!AboveMinExclusive(InputFileMap, var, 0)){
    std::cerr << "Couldn't read " << var <<  std::endl;
    return false;
  }
  return true;

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
//    YAML::Emitter emitter;
//    emitter << config;
//    std::cout<<emitter.c_str()<<std::endl;
// Read in attributes of a particular cavity
    const YAML::Node& cav = *it;
    std::unordered_map<std::string,std::string> AttrMap;
    for(YAML::const_iterator attr = cav.begin(); attr != cav.end(); ++attr) {
      std::string key = attr->first.as<std::string>();
      std::string str_val = attr->second.as<std::string>();
      AttrMap.insert({key,str_val});
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
        if(TestDouble(AttrMap,"r") && AboveMinInclusive(AttrMap,"order",1) && SandwichedBetweenInclusive(AttrMap,"Phase",-180,180)){
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
        if(TestDouble(AttrMap,"shunt") && AboveMinInclusive(AttrMap,"order",1) && SandwichedBetweenInclusive(AttrMap,"Phase",-180,180)){
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
  } // done iterating through all cavities
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
