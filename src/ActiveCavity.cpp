#include "ActiveCavity.h"
#include "Bunch.h"
#include "Constants.h"
#include <iostream>
#include <unordered_map>
#include <string>
#include <cmath>

ActiveCavity::ActiveCavity(std::string cav_name, double relative_voltage, double CavityPhase, double BLA, int CavityOrder, double CavityBeta){
  name = cav_name;
  cav_type = ACTIVE;
  r = relative_voltage;
  Phase= CavityPhase*pi/180.0; // convert to radians
  BeamLoadingAngle = BLA;
  order = CavityOrder;
  beta= CavityBeta;
};

double ActiveCavity::Voltage(double tau, const Bunch bunch, Parameters Para){
  double Vrf, omega0;
  int nharm;
  bool read_success = Para.get_parameter("vrf", Vrf) &&
      Para.get_parameter("nharm", nharm) &&
      Para.get_parameter("omega0", omega0);
  if(!read_success){
    throw std::runtime_error("Couldn't read parameters for Active cavity " + name);
  }
  return Vrf*sin(order*nharm*omega0*tau/time_scaling_factor+Phase);
}

void ActiveCavity::print(){
  std::cout << "Cavity: " << name << '\t' << "Type: Active" << "\t" << "RelVoltage:" << r << '\t' << "Phase: " << Phase << '\t' << "BeamLoadingAngle: " << BeamLoadingAngle << '\t' << "Order: " <<  order << " Beta: " << beta << std::endl;
}