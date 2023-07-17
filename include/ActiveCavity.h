#ifndef ACTIVE_CAVITY
#define ACTIVE_CAVITY
#include "Cavity.h"
#include <iostream>
#include <unordered_map>
#include <string>

class ActiveCavity: public Cavity{
  private:
    double r; // relative voltage
  public:
    ActiveCavity(std::string cav_name, double relative_voltage, double Phase, double order);
    // Voltage has this signature to be consistant with PassiveCavity class
    double Voltage(double tau, const Bunch bunch, const std::unordered_map<std::string,std::string> ParameterMap) override;

    void print() override;
};
#endif
