#include "ActiveCavity.h"
#include "Bunch.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>

ActiveCavity::ActiveCavity(std::string cav_name, double ref_v, int n_harm, double base_freq,  double relative_voltage, double CavityPhase, int CavityOrder){
  Vrf = ref_v;
  nharm =n_harm;
  frf = base_freq;
  name = cav_name;
  r = relative_voltage;
  Phase= CavityPhase;
  order = CavityOrder;
};

double ActiveCavity::Voltage(double tau, const Bunch bunch, const std::unordered_map<std::string,std::string> ParameterMap){
  
  return 0.0;
}

void ActiveCavity::print(){
  std::cout << "Cavity: " << name << '\t' << "Type: Active" << "\t" << "RelVoltage:" << r << '\t' << "Phase: " << Phase << '\t' << "Order: " <<  order << std::endl;
}