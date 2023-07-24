#include <iostream>
#include <unordered_map>
#include <string>

#include "Constants.h"
#include "ValidateInput.h"
#include "Parameters.h"

bool Parameters::add_parameter(std::string name,std::string candidate_value, Type t){
// make sure that key doesn't already exist
  if(parameters.count(name)!=0){
    std::cerr << name << " already exists in parameter list" << std::endl;
    return false;
  }
  parameters[name] = std::make_tuple(candidate_value,t);
  return true;
}

bool Parameters::remove_parameter(std::string name){
// make sure that key does exist
  if(parameters.count(name)==0){
    std::cerr << name << " doesn't exists in parameter list" << std::endl;
    return false;
  }
  parameters.erase(name);
  return true;

}

bool Parameters::get_parameter(std::string name, int& var){
// make sure that key does exist
  auto it = parameters.find(name);
  if(it == parameters.end()){
    std::cerr << name << " doesn't exists in parameter list" << std::endl;
    return false;
  }
  std::tuple<std::string, Type> val_tup  = it->second;
  std::string val = std::get<0>(val_tup);
  if(std::get<1>(val_tup) != INT){
    std::cerr << val << " is not allowed to be converted to an integer" << std::endl;
    return false;
  }
  double inter_val;
  StrToDouble(val,inter_val);
  var = (int) inter_val;
  return true;
}
bool Parameters::get_parameter(std::string name, double& var){
// make sure that key does exist
  auto it = parameters.find(name);
  if(it == parameters.end()){
    std::cerr << name << " doesn't exists in parameter list" << std::endl;
    return false;
  }
  std::tuple<std::string, Type> val_tup  = it->second;
  std::string val = std::get<0>(val_tup);
  if(std::get<1>(val_tup) != DOUBLE){
    std::cerr << val << " is not allowed to be converted to an double" << std::endl;
    return false;
  }
  StrToDouble(val,var);
  return true;
}

bool Parameters::get_parameter(std::string name, std::string& var){
// make sure that key does exist
  auto it = parameters.find(name);
  if(it == parameters.end()){
    std::cerr << name << " doesn't exists in parameter list" << std::endl;
    return false;
  }
  std::tuple<std::string, Type> val_tup  = it->second;
  std::string val = std::get<0>(val_tup);
  if(std::get<1>(val_tup) != STRING){
    std::cerr << val << " is not allowed to be converted to a string" << std::endl;
    return false;
  }
  var = val;
  return true;
}

void Parameters::print(){
  for(auto pair: parameters){
    std::string key = pair.first;
    auto tup =  pair.second;
    std::string val = std::get<0>(tup);
    Type t = std::get<1>(tup);
    std::string type_str;
    std::string converted_str;
    switch(t){
      case INT:{
        type_str = "INT";
        double temp;
        StrToDouble(val,temp);
        int int_temp = (int) temp;
        converted_str = std::to_string(int_temp);
        break;
      }
      case DOUBLE:{
        type_str = "DOUBLE";
        double temp;
        StrToDouble(val,temp);
        converted_str = std::to_string(temp);
        break;
      }
      case STRING:{
        type_str = "STRING";
        converted_str = val;
        break;
      }
      default:{
        std::runtime_error("Invalid type for key " + key);
        break;
      }
    }
    std::cout << key << '\t' << val << '\t' << type_str << std::endl;
  }
}