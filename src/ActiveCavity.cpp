#include "ActiveCavity.h"
#include "Bunch.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>

ActiveCavity::ActiveCavity(std::string cav_name, double relative_voltage, double CavityPhase, double CavityOrder){
  name = cav_name;
  r = relative_voltage;
  Phase= CavityPhase;
  order = CavityOrder;
};

double ActiveCavity::Voltage(double tau, const Bunch bunch, const std::unordered_map<std::string,std::string> ParameterMap){
  
  return 0.0;
}

void ActiveCavity::print(){
  std::cout << "Cavity: " << name << '\t' << "Type: Active" << "\t" << "RelVol:" << r << '\t' << "Phase: " << Phase << '\t' << "Order: " <<  order << std::endl;
}