#include "PassiveCavity.h"
#include "Bunch.h"
#include <iostream>
#include <unordered_map>
#include <string>

PassiveCavity::PassiveCavity(std::string cav_name, double shunt, double qual_f, double det_freq, double CavityOrder){
  name = cav_name;
  shunt_impedance = shunt;
  quality_factor = qual_f;
  detune_freq = det_freq;
  if(CavityOrder <1.0){
    std::runtime_error("Cavity order is not at least 1");
  }
  order = CavityOrder;
};

double PassiveCavity::Voltage(double tau,const  Bunch bunch, const std::unordered_map<std::string,std::string> ParameterMap){
  return 0.0;
}

void PassiveCavity::print(){
  std::cout << "Cavity: " << name << " Type: Passive Order: " << order << " Shunt Impedance: " << shunt_impedance << " Q: " << quality_factor << " detune: " << detune_freq << std::endl;
}