#include "PassiveCavity.h"
#include "Bunch.h"
#include "Constants.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>

PassiveCavity::PassiveCavity(std::string cav_name, double unloaded_shunt, double unloaded_qual_f, double det_freq, int CavityOrder, double beta){
  name = cav_name;
  unloaded_shunt_impedance = unloaded_shunt;
  unloaded_quality_factor = unloaded_qual_f;
  detune_freq = det_freq;
  order = CavityOrder;
};

double PassiveCavity::Voltage(double tau, const Bunch bunch, Parameters Para){
// Assumes uniform filling. Couldn't find a formula for general case
  return 0.0;
}

void PassiveCavity::print(){
  std::cout << "Cavity: " << name << " Type: Passive Order: " << order << " Shunt Impedance: " << unloaded_shunt_impedance << " Q: " << unloaded_quality_factor << " detune: " << detune_freq << " Beta: " << beta << std::endl;
}