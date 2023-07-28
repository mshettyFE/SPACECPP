#include <iostream>
#include <unordered_map>
#include <string>
#include <type_traits>

#include "Constants.h"
#include "ValidateInput.h"
#include "Parameters.h"

#include "doctest.h"

bool Parameters::add_parameter(std::string name,std::string candidate_value, Type t){
// add key to parameter after type checking
  if(parameters.count(name)!=0){
    std::cerr << name << " already exists in parameter list" << std::endl;
    return false;
  }
  switch(t){
    case INT:
      try{
        std::stoi(candidate_value);
      }
      catch(...){
        std::cerr << "Couldn't convert " << candidate_value << " to int" << std::endl;
        return false;
      }
      break;
    case DOUBLE:
        double temp;
        if(!StrToDouble(candidate_value,temp)){
        std::cerr << "Couldn't convert " << candidate_value << " to double" << std::endl;
        return false;
      }
      break;
    case STRING:
      break;
    default:
      std::cerr << "Invalid type" << std::endl;
      return false;
  }
  parameters[name] = std::make_tuple(candidate_value,t);
  return true;
}

TEST_CASE("Testing add_parameter in Parameters class"){
  Parameters para = Parameters();
  CHECK(para.add_parameter("one","5",INT)== true);
  // checking if key already exists
  CHECK(para.add_parameter("one","5.0",DOUBLE)== false);
  CHECK(para.add_parameter("two","not_an_int",INT) == false);
  CHECK(para.add_parameter("three","1353456.2353", DOUBLE) == true);
  CHECK(para.add_parameter("four", "not_a_double", DOUBLE) == false);
  CHECK(para.add_parameter("five","sample", STRING) == true);
}

bool Parameters::remove_parameter(std::string key){
// remove key-value pair from parameters
  if(parameters.count(key)==0){
    std::cerr << key << " doesn't exists in parameter list" << std::endl;
    return false;
  }
  parameters.erase(key);
  return true;
}

TEST_CASE("Testing remove_parameter in Parameters class"){
  Parameters para = Parameters();
  CHECK(para.remove_parameter("Test_key")==false);
  para.add_parameter("Test_key","sample", STRING);
  CHECK(para.remove_parameter("Test_key")==true);
}

template <typename Output>
bool Parameters::get_parameter(std::string name, Output& var){
// Generic function that extracts a parameter if it exists
  auto it = parameters.find(name);
  if(it == parameters.end()){
    std::cerr << name << " doesn't exists in parameter list" << std::endl;
    return false;
  }
  std::tuple<std::string, Type> val_tup  = it->second;
  return match_type(val_tup, var);
}

// templated classes that deal with retreiving INT, DOUBLE, STRING from parameters
template<>
bool Parameters::match_type<int>(std::tuple<std::string, Type> val_tup, int& var){
      if(std::get<1>(val_tup) != INT){
        std::cerr << std::get<0>(val_tup) << " is not allowed to be converted to an integer" << std::endl;
        return false;
      }
      var = std::stoi(std::get<0>(val_tup));
      return true;
}

template<>
bool Parameters::match_type<double>(std::tuple<std::string, Type> val_tup, double& var){
      if(std::get<1>(val_tup) != DOUBLE){
        std::cerr << std::get<0>(val_tup) << " is not allowed to be converted to an double" << std::endl;
        return false;
      }
      StrToDouble(std::get<0>(val_tup),var);
      return true;
}

template<>
bool Parameters::match_type<std::string>(std::tuple<std::string, Type> val_tup, std::string& var){
      if(std::get<1>(val_tup) != STRING){
        std::cerr << std::get<0>(val_tup) << " is not allowed to be converted to a string" << std::endl;
        return false;
      }
      var = std::get<0>(val_tup);
      return true;
}

TEST_CASE("Testing get_parameter in Parameters class"){
  Parameters para = Parameters();
  para.add_parameter("first","5",INT);
  para.add_parameter("second","5.9",DOUBLE);
  para.add_parameter("third","sample",STRING);
  int temp_int;
  double temp_double;
  std::string temp_str;
  CHECK(para.get_parameter("four", temp_str)==false);
  CHECK(para.get_parameter("first", temp_int)==true);
  CHECK(para.get_parameter("first", temp_double)==false);
  CHECK(para.get_parameter("second", temp_double)==true);
  CHECK(para.get_parameter("second", temp_int)==false);
  CHECK(para.get_parameter("third", temp_str)==true);
  CHECK(para.get_parameter("third", temp_int)==false);
}

void Parameters::print(){
// Prints contents of parameters
  std::cout << "Key\tInternal String\tType\tOutput String" <<  std::endl;
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
    std::cout << key << '\t' << val << '\t' << type_str << '\t' << converted_str << std::endl;
  }
}