#include "PassiveCavity.h"
#include "Bunch.h"
#include "Constants.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>

PassiveCavity::PassiveCavity(std::string cav_name, double unloaded_shunt, double unloaded_qual_f, double det_freq, int CavityOrder, double CavityBeta){
  name = cav_name;
  cav_type = PASSIVE;
  unloaded_shunt_impedance = unloaded_shunt;
  unloaded_quality_factor = unloaded_qual_f;
  detune_freq = det_freq;
  order = CavityOrder;
  beta = CavityBeta;
};

double PassiveCavity::Voltage(double tau, const Bunch bunch, Parameters Para){
// Assumes uniform filling. Couldn't find a formula for general case
// see https://accelconf.web.cern.ch/icap2018/papers/tupaf12.pdf
  double I0, omega0;
  int nharm;
  bool check1 = Para.get_parameter("Aver_curr",I0);
  bool check2 = Para.get_parameter("omega0",omega0);
  bool check3 = Para.get_parameter("nharm",nharm);
  if(!(check1 && check2 && check3)){
    throw std::runtime_error("Couldn't read in Global parameters for Passive cavity voltage");
  }
  double Rs = unloaded_shunt_impedance/(1+beta);
  double Q = unloaded_quality_factor/(1+beta);
  double cavity_freq = omega0*order*nharm;
  double Phi = atan(2*Q*detune_freq/cavity_freq);
  double sigma_tau_squared = bunch.MomentGeneratorTau(2);
  double I_m = 2*I0*exp(-0.5*cavity_freq*cavity_freq*sigma_tau_squared);
  return -I_m*Rs*cos(Phi)*cos(Phi+tau/time_scaling_factor*cavity_freq);
}

void PassiveCavity::print(){
  std::cout << "Cavity:\t" << name << " Type: Passive\tOrder: " << order << "\tShunt Impedance: " << unloaded_shunt_impedance << "\tQ: " << unloaded_quality_factor << "\tdetune: " << detune_freq << "\tBeta: " << beta << std::endl;
}