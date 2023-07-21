#include "PassiveCavity.h"
#include "Bunch.h"
#include "Constants.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>

PassiveCavity::PassiveCavity(std::string cav_name, int n_harm, double base_freq, double shunt, double qual_f, double det_freq, int CavityOrder){
  nharm =n_harm;
  frf = base_freq;
  name = cav_name;
  shunt_impedance = shunt;
  quality_factor = qual_f;
  detune_freq = det_freq;
  order = CavityOrder;
  Phi = atan(detune_freq/(order*nharm*frf));
};

double PassiveCavity::Voltage(double tau, const Bunch bunch){
// Assumes uniform filling. Couldn't find a formula for general case
  return 0.0;
}

void PassiveCavity::print(){
  std::cout << "Cavity: " << name << " Type: Passive Order: " << order << " Shunt Impedance: " << shunt_impedance << " Q: " << quality_factor << " detune: " << detune_freq << std::endl;
}