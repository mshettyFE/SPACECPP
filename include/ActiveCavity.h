#ifndef ACTIVE_CAVITY
#define ACTIVE_CAVITY
#include "Cavity.h"
#include <iostream>
#include <unordered_map>
#include <string>

class ActiveCavity: public Cavity{
  private:
    double Vrf; // reference voltage of main cavity
    double r; // relative voltage
    double Phase;
  public:
    ActiveCavity(std::string cav_name, double ref_v, int n_harm, double base_freq,  double relative_voltage, double CavityPhase, int CavityOrder);
    // Voltage has this signature to be consistant with PassiveCavity class
    // Need to verify ParameterMap when you load it from file (see FileParser.h)
    double Voltage(double tau, const Bunch bunch, const std::unordered_map<std::string,std::string> ParameterMap) override;
    // print parameters of cavity
    void print() override;
};
#endif
