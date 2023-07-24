#include<fstream>
#include<iostream>
#include<sstream>
#include<string>
#include<vector>
#include <memory>
#include<utility>
#include<unordered_map>
#include <cmath>
#include "yaml-cpp/yaml.h"

#include "Cavity.h"
#include "Constants.h"
#include "ActiveCavity.h"
#include "PassiveCavity.h"
#include "ValidateInput.h"
#include "FileParser.h"
#include "Parameters.h"

bool ValidateYAMLWrapper(Parameters& Para, YAML::Node CurNode, std::string key, Type t, ValidityCheckingFlags flag, double boundary1, double boundary2){
  auto it = CurNode[key];
  if(!it){
    std::cerr << "Couldn't read " << key << std::endl;
    return false;
  }
  std::string val  = it.as<std::string>();
  if(!ValidityChecking( val , flag, boundary1, boundary2)){
    std::cerr << "Couldn't parse " << val << std::endl;
    return false;
  }
  Para.add_parameter(key,val,t);
  return true;
}

bool ValidateUnorderedMapWrapper(Parameters& Para, std::unordered_map<std::string,std::string> map,std::string key, Type t, ValidityCheckingFlags flag, double boundary1, double boundary2){
  if(map.count(key) ==0){
    std::cerr << "Couldn't read " << key << std::endl;
    return false;
  }
  std::string val  = map[key];
  if(!ValidityChecking( val , flag, boundary1, boundary2)){
    std::cerr << "Couldn't parse value of " << key << std::endl;
    return false;
  }
  Para.add_parameter(key,val,t);
  return true;
}



bool ReadLatticeParameters(std::string fname, Parameters& Para){
  std::cout << "Parsing " << fname << std::endl;
// Read in contents of .yaml file containing lattice parameters
  YAML::Node config;
  try{
    config = YAML::LoadFile(fname);
  }
  catch(...){
    std::cerr << "Couldn't parse config file: " << fname <<  std::endl;
    return false;
  }
  bool output = 
// E0 must be greater than 0
    ValidateYAMLWrapper(Para, config, "E0", DOUBLE, MIN_EXCLUSIVE, 0) &&
// vrf must be greater than 0
    ValidateYAMLWrapper(Para, config, "vrf", DOUBLE, MIN_EXCLUSIVE, 0) &&
// nharm is an integer that is greater than 1
    ValidateYAMLWrapper(Para, config, "nharm", INT, MIN_INCLUSIVE, 1) &&
// frf must be greater than 0
    ValidateYAMLWrapper(Para, config, "frf", DOUBLE, MIN_EXCLUSIVE, 0) &&
// Aver_curr must be greater than 0
    ValidateYAMLWrapper(Para, config, "Aver_curr", DOUBLE, MIN_EXCLUSIVE, 0) &&
// gap is an integer that is atleast 0
    ValidateYAMLWrapper(Para, config, "gap", INT, MIN_INCLUSIVE, 0) &&
// sig_d must be greater than 0
    ValidateYAMLWrapper(Para, config, "sig_d", DOUBLE, MIN_EXCLUSIVE, 0) &&
// alpha_C must be greater than 0
    ValidateYAMLWrapper(Para, config, "alpha_C", DOUBLE, MIN_EXCLUSIVE, 0) &&
// Uloss must be at least 0
    ValidateYAMLWrapper(Para, config, "Uloss", DOUBLE, MIN_INCLUSIVE, 0) &&
// rjs must be greater than 0
    ValidateYAMLWrapper(Para, config, "rjs", DOUBLE, MIN_EXCLUSIVE, 0);
  return output;
}

bool ReadTimeEvolutionParameters(std::string fname, Parameters& Para){
  std::cout << "Parsing " << fname << std::endl;
// Read in contents of .yaml file containing lattice parameters
  YAML::Node config;
  try{
    config = YAML::LoadFile(fname);
  }
  catch(...){
    std::cerr << "Couldn't parse config file: " << fname <<  std::endl;
    return false;
  }
  bool output = 
// nturns must be an integer which is at least 1
    ValidateYAMLWrapper(Para, config, "nturns", INT, MIN_INCLUSIVE, 1) &&
// kon must be an integer that is at least 0
    ValidateYAMLWrapper(Para, config, "kon", INT, MIN_INCLUSIVE, 0) &&
// frad must be at least 0
    ValidateYAMLWrapper(Para, config, "frad", DOUBLE, MIN_INCLUSIVE, 0) &&
// nturnon must be at least 0 and an integer
    ValidateYAMLWrapper(Para, config, "nturnon", INT, MIN_INCLUSIVE, 0) &&
// nhist must be at least 1 and an integer
    ValidateYAMLWrapper(Para, config, "nhist", INT, MIN_INCLUSIVE, 1);
  return output;
}

bool ReadWakefieldParameters(std::string fname, Parameters& Para){
  std::cout << "Parsing " << fname << std::endl;
// Read in contents of .yaml file containing lattice parameters
  YAML::Node config;
  try{
    config = YAML::LoadFile(fname);
  }
  catch(...){
    std::cerr << "Couldn't parse config file: " << fname <<  std::endl;
    return false;
  }
  bool output = 
// rangetau must be greater than 0
    ValidateYAMLWrapper(Para, config, "rangetau", DOUBLE, MIN_EXCLUSIVE, 0) &&
//Fourier Coefficients must be an integer and at least 1
    ValidateYAMLWrapper(Para, config, "FourierCoeff", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "ng1", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "ng2", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "ng1g2", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "npart", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "ngridd", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "NJ", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "NG", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "NTY", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "nup", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "nup1", INT, MIN_INCLUSIVE, 1) &&
    ValidateYAMLWrapper(Para, config, "nharmh", INT, MIN_INCLUSIVE, 1);
  return output;
}

bool ReadCavityParameters(std::string fname, std::vector<std::unique_ptr<Cavity>>& cavities){
  std::cout << "Parsing " << fname << std::endl;
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
  for( YAML::const_iterator it = cavData.begin(); it != cavData.end(); ++it) {
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
        Parameters TempP = Parameters();
        bool suc_check= 
            ValidateUnorderedMapWrapper(TempP, AttrMap,"r", DOUBLE, IS_VALID) &&
ValidateUnorderedMapWrapper(TempP, AttrMap,"order", INT, MIN_INCLUSIVE, 1) &&
ValidateUnorderedMapWrapper(TempP, AttrMap,"Phase", DOUBLE, MIN_INCLUSIVE_MAX_INCLUSIVE, -180, 180) &&
ValidateUnorderedMapWrapper(TempP, AttrMap,"Load", DOUBLE, MIN_INCLUSIVE_MAX_INCLUSIVE, -180, 180) &&
ValidateUnorderedMapWrapper(TempP, AttrMap,"Beta", DOUBLE, MIN_INCLUSIVE,0) ;
        if(suc_check && AttrMap.count("name")!=0){            
            std::string cname = AttrMap.at("name");
            double r, Phase, BLA, beta;
            int Order;
            bool assign = TempP.get_parameter("r",r) &&
            TempP.get_parameter("Phase",Phase) &&
            TempP.get_parameter("Load",BLA) &&
            TempP.get_parameter("order",Order) &&
            TempP.get_parameter("Beta",beta);
            if(assign){
              cavities.push_back(std::make_unique<ActiveCavity>(cname, r, Phase, BLA, Order,beta));          
            }
            else{
              return false;
            }
        }
        else{
          return false;
        }
    }
    else if(AttrMap.at("type")=="passive"){
        Parameters TempP = Parameters();
        bool suc_check= 
            ValidateUnorderedMapWrapper(TempP, AttrMap,"shunt", DOUBLE, MIN_EXCLUSIVE, 0)&&
            ValidateUnorderedMapWrapper(TempP, AttrMap,"order", INT, MIN_INCLUSIVE, 1) &&
            ValidateUnorderedMapWrapper(TempP, AttrMap,"detuning", DOUBLE, IS_VALID) &&
            ValidateUnorderedMapWrapper(TempP, AttrMap,"quality", DOUBLE, MIN_EXCLUSIVE, 0) &&
            ValidateUnorderedMapWrapper(TempP, AttrMap,"Beta", DOUBLE, MIN_INCLUSIVE,0);
        if(suc_check && AttrMap.count("name")!=0){            
            std::string cname = AttrMap.at("name");
            double shunt, qual, detune, beta;
            int Order;
            bool assign = TempP.get_parameter("shunt",shunt) &&
            TempP.get_parameter( "detuning",detune) &&
            TempP.get_parameter("quality",qual) &&
            TempP.get_parameter("order",Order) &&
            TempP.get_parameter("Beta",beta);
            if(assign){
              cavities.push_back(std::make_unique<PassiveCavity>(cname, shunt, qual, detune, Order,beta));          
            }
            else{
              return false;
            }
        }
// We check that shunt is strictly positive, order is at least 1, detuning frequency is a valid double, quality factor is stritly positive
        else{
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
bool ReadBunchParameters(std::string fname, std::vector<Bunch>& bunches ){
  std::cout << "Parsing " << fname << std::endl;
// Read in contents of .yaml file containing bunch data
  YAML::Node config;
  try{
    config = YAML::LoadFile(fname);
  }
  catch(...){
    std::cerr << "Couldn't parse config file: " << fname <<  std::endl;
    return false;
  }
  Parameters TempPara = Parameters();
  bool suc_check = 
  ValidateYAMLWrapper(TempPara, config, "npop", INT, MIN_INCLUSIVE, 1) &&
  ValidateYAMLWrapper(TempPara, config, "nbunches", INT, MIN_INCLUSIVE, 1);
  if(!suc_check){
    std::cerr << "Couldn't read either npop or nbunches " << std::endl;
    return false;
  }
  int nbunches, npop;
  TempPara.get_parameter("nbunches", nbunches);
  TempPara.get_parameter("npop", npop);
  if(!ValidateYAMLWrapper(TempPara, config, "gap", INT, MIN_INCLUSIVE_MAX_EXCLUSIVE, 0, nbunches)){
    std::cerr << "Couldn't parse gap variable in " << fname <<  std::endl;
    return false;
  }
  int gap;
  TempPara.get_parameter("gap", gap);

  YAML::Node RandVarNode = config["random_vars"];
  if(!RandVarNode){
    std::cerr << "Couldn't locate random_vars tag in " << fname << std::endl;
    return false;
  }
  TempPara = Parameters();
    bool valid = ValidateYAMLWrapper(TempPara, RandVarNode, "tau_mu", DOUBLE, IS_VALID)&&
        ValidateYAMLWrapper(TempPara, RandVarNode, "tau_sigma", DOUBLE, MIN_EXCLUSIVE, 0) &&
         ValidateYAMLWrapper(TempPara, RandVarNode, "delta_mu", DOUBLE, IS_VALID)&&
        ValidateYAMLWrapper(TempPara, RandVarNode, "delta_sigma", DOUBLE, MIN_EXCLUSIVE, 0) &&
         ValidateYAMLWrapper(TempPara, RandVarNode, "x_trans_mu", DOUBLE, IS_VALID)&&
        ValidateYAMLWrapper(TempPara, RandVarNode, "x_trans_sigma", DOUBLE, MIN_EXCLUSIVE, 0) &&
         ValidateYAMLWrapper(TempPara, RandVarNode, "px_trans_mu", DOUBLE, IS_VALID)&&
        ValidateYAMLWrapper(TempPara, RandVarNode, "px_trans_sigma", DOUBLE, MIN_EXCLUSIVE, 0);
    if(!valid){
      std::cerr << "Couldn't parse input random variable parameters from " << fname << std::endl;
      return false;
    }
  std::unordered_map<Coords, std::tuple<double,double>> coord_parameters;
  double t1,t2;
  TempPara.get_parameter("tau_mu",t1);
  TempPara.get_parameter("tau_sigma",t2);
  coord_parameters[TAU] = std::make_tuple(t1,t2);
  //delta
  TempPara.get_parameter("delta_mu",t1);
  TempPara.get_parameter("delta_sigma",t2);
  coord_parameters[DELTA] = std::make_tuple(t1,t2);
  //x_Trans
  TempPara.get_parameter("x_trans_mu",t1);
  TempPara.get_parameter("x_trans_sigma",t2);
  coord_parameters[X_TRANS] = std::make_tuple(t1,t2);
  //px_Trans
  TempPara.get_parameter("px_trans_mu",t1);
  TempPara.get_parameter("px_trans_sigma",t2);
  coord_parameters[PX_TRANS] = std::make_tuple(t1,t2);
  for(int i=0; i< nbunches-gap; ++i){
    bunches.push_back(Bunch(npop,coord_parameters));
  }
  for(int i=nbunches-gap; i< nbunches; ++i){
    bunches.push_back(Bunch(0,coord_parameters));
  }
  return true;
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
