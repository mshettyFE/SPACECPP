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
      std::cerr << "No value detected" << std::endl;
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
              std::cerr << "Negative sign can only appear at the beginning for numbers not in scientific notation" << std::endl;
              return false;
            }
          }
          else{
            std::cerr << "Invalid character at location " << location << " in value " << parameter_value_str << std::endl;
            return false;
          }
        }
        // try-catch for just in case the above misses something
        try{
          out_value = std::stod(parameter_value_str);
        }
        catch(...){
            std::cerr << "Couldn't convert value to double" << std::endl;
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
          // If E_loc hasn't been set to 1 yet, then E_loc+1=0 or the first character
          if(current_loc==(E_loc+1)){
            continue;
          }
          else{
            std::cerr << "Negative sign in incorrect place " << std::endl;
            return false;        
          }
        }
      // All other characters must be valid digits
        else if(isdigit(character)){
          continue;
        }
        else{
            std::cerr << "Invalid character at location " << current_loc << " in value " << parameter_value_str << std::endl;
          return false;
        }
      }
    // try-catch for just in case the above misses something
      try{
        out_value = std::stod(parameter_value_str);
      }
      catch(...){
          std::cerr << "Couldn't convert value to double" << std::endl;
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
    double temp;
    if(!StrToDouble(map[key],temp)){
      std::cerr << "Couldn't convert " << map.at(key) << " to a double" << std::endl;
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
/*
  Used to validate that a value conforms to a constraint. Suppose that our number in question is x. We have 9 possible cases:
      * IS_VALID: Only checks if an value of a key is a valid real number
      The rest do validity checking in addition to boundary checking
      * MIN_EXCLUSIVE: checks if value is strictly greater than boundary1
      * MIN_INCLUSIVE: checks if value is greater than or equal to boundary1
      * MAX_EXCLUSIVE: checks if value is strictly less than boundary1
      * MAX_INCLUSIVE: checks if value is less than or equal to boundary1
      * MIN_EXCLUSIVE_MAX_EXCLUSIVE: checks if value is strictly greater than boundary1 and strictly less than boundary2
      * MIN_INCLUSIVE_MAX_INCLUSIVE:checks if value is  greater than or equal to boundary1 and less than or equal to boundary2
      * MIN_EXCLUSIVE_MAX_INCLUSIVE: checks if value is strictly greater than boundary1 and less than or equal to boundary2
      *MIN_INCLUSIVE_MAX_EXCLUSIVE: checks if value is greater than or equal to boundary1 and strictly less than boundary2
*/
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

TEST_CASE("Testing ValidityCheck...") {
    std::unordered_map<std::string,std::string> test_map;
    double min_val, max_val;
    // Is valid check
    test_map["one"] = "not_valid";
    CHECK(ValidityChecking(test_map, "one",IS_VALID) == true);
    test_map["one"] = "1231.23423";
    CHECK(ValidityChecking(test_map, "one",IS_VALID) == true);
    // Min exclusive check
    test_map["two"] = "234.5671";
    min_val = 100;
    CHECK(ValidityChecking(test_map, "two", MIN_EXCLUSIVE,min_val) == true);
    test_map["two"] = "100";
    CHECK(ValidityChecking(test_map, "two", MIN_EXCLUSIVE,min_val) == false);
    test_map["two"] = "10.1234";
    CHECK(ValidityChecking(test_map, "two", MIN_EXCLUSIVE,min_val) == false);
    // Min inclusive check
    test_map["three"] = "234.5671";
    min_val = 100;
    CHECK(ValidityChecking(test_map, "three", MIN_INCLUSIVE,min_val) == true);
    test_map["three"] = "100";
    CHECK(ValidityChecking(test_map, "three", MIN_INCLUSIVE,min_val) == true);
    test_map["three"] = "10.1234";
    CHECK(ValidityChecking(test_map, "three", MIN_INCLUSIVE,min_val) == false);
    // Max exclusive check
    test_map["four"] = "1234.5671";
    max_val = 1000;
    CHECK(ValidityChecking(test_map, "four", MAX_EXCLUSIVE,max_val) == false);
    test_map["four"] = "100";
    CHECK(ValidityChecking(test_map, "four", MAX_EXCLUSIVE,max_val) == true);
    test_map["four"] = "1000";
    CHECK(ValidityChecking(test_map, "four", MAX_EXCLUSIVE,max_val) == false);
    // Min inclusive check
    test_map["five"] = "1234.5671";
    max_val = 1000;
    CHECK(ValidityChecking(test_map, "five", MAX_INCLUSIVE,max_val) == false);
    test_map["five"] = "100";
    CHECK(ValidityChecking(test_map, "five", MAX_INCLUSIVE,max_val) == true);
    test_map["three"] = "1000";
    CHECK(ValidityChecking(test_map, "three", MAX_INCLUSIVE,max_val) == true);
    // min exclusive max exclusive
    test_map["six"] = "503.6";
    min_val = 10;
    max_val = 1000;
    CHECK(ValidityChecking(test_map, "six", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == true);
    test_map["six"] = "10";
    CHECK(ValidityChecking(test_map, "six", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    test_map["six"] = "9.9";
    CHECK(ValidityChecking(test_map, "six", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    test_map["six"] = "1000";
    CHECK(ValidityChecking(test_map, "six", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    test_map["six"] = "1000.1";
    CHECK(ValidityChecking(test_map, "six", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    // min inclusive max inclusive
    test_map["seven"] = "503.6";
    min_val = 10;
    max_val = 1000;
    CHECK(ValidityChecking(test_map, "seven", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    test_map["seven"] = "10";
    CHECK(ValidityChecking(test_map, "seven", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    test_map["seven"] = "9.9";
    CHECK(ValidityChecking(test_map, "seven", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    test_map["seven"] = "1000";
    CHECK(ValidityChecking(test_map, "seven", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    test_map["seven"] = "1000.1";
    CHECK(ValidityChecking(test_map, "seven", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    // min exclusive max inclusive
    test_map["eight"] = "503.6";
    min_val = 10;
    max_val = 1000;
    CHECK(ValidityChecking(test_map, "eight", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    test_map["eight"] = "10";
    CHECK(ValidityChecking(test_map, "eight", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    test_map["eight"] = "9.9";
    CHECK(ValidityChecking(test_map, "eight", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    test_map["eight"] = "1000";
    CHECK(ValidityChecking(test_map, "eight", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    test_map["eight"] = "1000.1";
    CHECK(ValidityChecking(test_map, "eight", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    // min inclusive max exclusive
    test_map["nine"] = "503.6";
    min_val = 10;
    max_val = 1000;
    CHECK(ValidityChecking(test_map, "nine", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == true);
    test_map["nine"] = "10";
    CHECK(ValidityChecking(test_map, "nine", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == true);
    test_map["nine"] = "9.9";
    CHECK(ValidityChecking(test_map, "nine", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    test_map["nine"] = "1000";
    CHECK(ValidityChecking(test_map, "nine", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    test_map["nine"] = "1000.1";
    CHECK(ValidityChecking(test_map, "nine", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);    
}

bool ValidateInputs(std::unordered_map<std::string,std::string> InputFileMap){
// function that validates input parameters from .yaml file. Need to edit this function if we add more parameters
// Check that nturns exists and is greater than or equal to 1
   bool output =  (ValidityChecking(InputFileMap, "nturns", MIN_INCLUSIVE, 1) &&
// Check that npop exists and is strictly greater than or equal to 1
                    ValidityChecking(InputFileMap, "npop", MIN_INCLUSIVE, 1) &&
// Check that nbunches exists and is strictly greater than or equal to 1
                    ValidityChecking(InputFileMap, "nbunches", MIN_INCLUSIVE, 1) &&
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
        std::cerr << "Attempted to convert: " << str_val << " to C++ style double " << std::endl;
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

// Read in Vrf, nharm, frf
    YAML::Node VrfNode = config["Vrf"];
    YAML::Node nharmNode = config["nharm"];
    YAML::Node frfNode = config["frf"];
    if(!VrfNode){
      std::cerr << "Couldn't find Vrf in " << fname << std::endl;
    }
    if(!nharmNode){
      std::cerr << "Couldn't find nharm in " << fname << std::endl;
      return false;
    }
    if(!frfNode){
      std::cerr << "Couldn't find frf in " << fname << std::endl;
      return false;
    }
    
    std::unordered_map<std::string,std::string> GlobalCavityVariables;
    GlobalCavityVariables["Vrf"] = VrfNode.as<std::string>();
    GlobalCavityVariables["nharm"] = nharmNode.as<std::string>();
    GlobalCavityVariables["frf"] = frfNode.as<std::string>();
    for(auto item: GlobalCavityVariables){
        double temp;
        if(!StrToDouble(item.second,temp)){
            std::cerr << "Attempted to convert: " << item.second << " to C++ style double " << std::endl;
            return false;
        }    
    }

    // Make sure Vrf is at greater than 0, nharm is at least 1, and frf is greater than 0
    bool valid = ValidityChecking(GlobalCavityVariables, "Vrf", MIN_EXCLUSIVE, 0) &&
    ValidityChecking(GlobalCavityVariables, "nharm", MIN_INCLUSIVE, 1) &&
    ValidityChecking(GlobalCavityVariables, "frf", MIN_EXCLUSIVE, 0);
    if(!valid){
      std::cerr << "Couldn't parse Vrf, nharm, and frf in " << fname << std::endl;
      return false;
    }
    double Vrf, nharm_d, frf;
    StrToDouble(VrfNode.as<std::string>(),Vrf);
    StrToDouble(nharmNode.as<std::string>(),nharm_d);
    StrToDouble(frfNode.as<std::string>(),frf);
    int nharm = (int) nharm_d;
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
            int  Order = (int) std::stod(AttrMap.at("order"));
            cavities.push_back(std::make_unique<ActiveCavity>(cname,Vrf, nharm, frf, r, Phase,Order));
        }
        else{
          return false;
        }
      }
      else if(AttrMap.at("type")=="passive"){
// We check that shunt is strictly positive, order is at least 1, detuning frequency is a valid double, quality factor is stritly positive
        if(ValidityChecking(AttrMap, "shunt", MIN_EXCLUSIVE,0) &&
          ValidityChecking(AttrMap, "order", MIN_INCLUSIVE, 1) &&
           ValidityChecking(AttrMap, "detuning", IS_VALID) &&
          ValidityChecking(AttrMap, "quality", MIN_EXCLUSIVE, 0)){
            std::string cname = AttrMap.at("name");
            double shunt = std::stod(AttrMap.at("shunt"));
            int Order = (int) std::stod(AttrMap.at("shunt"));
            double qual = std::stod(AttrMap.at("quality"));
            double detune=  std::stod(AttrMap.at("detuning"));
            cavities.push_back(std::make_unique<PassiveCavity>(cname, nharm, frf, shunt, qual, detune, Order));
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
bool ReadBunchParameters(std::string fname, std::unordered_map<Coords, std::tuple<double,double>>& coord_parameters ){
// Read in contents of .yaml file containing cavity data
  YAML::Node config;
  try{
    config = YAML::LoadFile(fname);
  }
  catch(...){
    std::cerr << "Couldn't parse config file: " << fname <<  std::endl;
    return false;
  }
// Test for mu and sigma for all coordinates
    YAML::Node tau_mu = config["tau_mu"];
    YAML::Node tau_sigma = config["tau_sigma"];
    YAML::Node delta_mu = config["delta_mu"];
    YAML::Node delta_sigma = config["delta_sigma"];
    YAML::Node x_trans_mu = config["x_trans_mu"];
    YAML::Node x_trans_sigma = config["x_trans_sigma"];
    YAML::Node px_trans_mu = config["px_trans_mu"];
    YAML::Node px_trans_sigma = config["px_trans_sigma"];
    if(!(tau_mu && tau_sigma && delta_mu && delta_sigma && x_trans_mu && x_trans_sigma && px_trans_sigma && px_trans_sigma)){
      std::cerr << "Missing parameters to generate random initial particle position" <<  std::endl;
      return false;
    }

    std::unordered_map<std::string, std::string> TempMap;
    TempMap["tau_mu"] = tau_mu.as<std::string>();
    TempMap["tau_sigma"] = tau_sigma.as<std::string>();
    TempMap["delta_mu"] = delta_mu.as<std::string>();
    TempMap["delta_sigma"] = delta_sigma.as<std::string>();
    TempMap["x_trans_mu"] = x_trans_mu.as<std::string>();
    TempMap["x_trans_sigma"] = x_trans_sigma.as<std::string>();
    TempMap["px_trans_mu"] = px_trans_mu.as<std::string>();
    TempMap["px_trans_sigma"] = px_trans_sigma.as<std::string>();
    
    bool valid = 
        ValidityChecking(TempMap, "tau_mu", IS_VALID) &&
        ValidityChecking(TempMap, "tau_sigma", MIN_EXCLUSIVE, 0) &&
        ValidityChecking(TempMap, "delta_mu", IS_VALID) &&
        ValidityChecking(TempMap, "delta_sigma", MIN_EXCLUSIVE, 0) &&
        ValidityChecking(TempMap, "x_trans_mu", IS_VALID) &&
        ValidityChecking(TempMap, "x_trans_sigma", MIN_EXCLUSIVE, 0) &&
        ValidityChecking(TempMap, "px_trans_mu", IS_VALID) &&
        ValidityChecking(TempMap, "px_trans_sigma", MIN_EXCLUSIVE, 0);
  if(! valid){
    std::cerr << "couldn't parse parameters for random initial particle distribution " << std::endl;
    return false;
  }
  std::tuple<double,double> val = std::make_tuple(std::stod(TempMap["tau_mu"]),std::stod(TempMap["tau_sigma"]));
  coord_parameters[TAU] = val;
  val =  std::make_tuple(std::stod(TempMap["delta_mu"]),std::stod(TempMap["delta_sigma"]));
  coord_parameters[DELTA] = val;
  val = std::make_tuple(std::stod(TempMap["x_trans_mu"]),std::stod(TempMap["x_trans_sigma"]));
  coord_parameters[X_TRANS] = val;
  val = std::make_tuple(std::stod(TempMap["px_trans_mu"]),std::stod(TempMap["px_trans_sigma"]));
  coord_parameters[PX_TRANS] = val;
  return true;
}

void PrintInputMap(const std::unordered_map<std::string,std::string> Map){
    // Loop through all keys in map and print
    for(auto& pair: Map){
      std::cout << pair.first << '\t' << pair.second << std::endl;
    }
}

void PrintRandomGenMap(const std::unordered_map<Coords, std::tuple<double,double>> Map){
    for(auto& pair: Map){
      std::cout << pair.first << "\t mu: " << std::get<0>(pair.second) << "\t sigma: "  << std::get<1>(pair.second) <<  std::endl;
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
