#include "Wakefield.h"

#include <iostream>
#include <vector>
#include <tuple>
#include <string>
#include <fstream>
#include <algorithm>
#include <sstream>


Wakefield::Wakefield(std::string path){
  fname = path;
  read_file(path);
}

void Wakefield::print(){
  std::cout << fname << std::endl;
  for(auto pt : discrete_data){
    std::cout << std::get<0>(pt) << '\t' << std::get<1>(pt) << std::endl;
  }
}

void Wakefield::read_file(std::string path){
// open file and check it it exists
  std::ifstream f(path.c_str());
  if(!f.good()){
    f.close();
    std::string err_msg  = path + " does not exist";
    throw std::runtime_error(err_msg);
  }
// start reading line by line
  std::string line;
  while (std::getline(f, line))
  {
// pass string to string stream, then read first two columns, throwing exception if they aren't valid doubles
    std::istringstream iss(line);
    std::string buffer;
    double tau, value;
    try{
      std::getline(iss, buffer, ',');
      tau = std::stod(buffer);
      std::getline(iss, buffer, ',');
      value = std::stod(buffer);
    }
    catch(...){
      std::string err_msg  = "Couldn't parse " + path;
      throw std::runtime_error(err_msg);
    }
// put pair in data vector
    std::tuple<double,double> pair = std::make_tuple(tau,value);
    discrete_data.push_back(pair);
  }
// sort vector in ascending order, just in case they aren't in order
  std::sort(discrete_data.begin(), discrete_data.end()); 
  double min_tau = std::get<0>(discrete_data[0]);
// Make sure wakefield only defined on tau >0
  if(min_tau <0){
    std::string err_msg  = path + " is defined on tau values less than 0";
    throw std::runtime_error(err_msg);
  }
}
