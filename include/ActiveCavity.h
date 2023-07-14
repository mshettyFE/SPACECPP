#ifndef ACTIVE_CAVITY
#define ACTIVE_CAVITY
#include "Cavity.h"
#include <unordered_map>
#include <string>

class ActiveCavity: public Cavity{
  private:
    double r; // relative voltage
  public:
    ActiveCavity(double relative_voltage, double Phase, double order, std::unordered_map<std::string,double> map);
    // Voltage has this signature to be consistant with PassiveCavity class
    double Voltage(double tau, Bunch bunch);
};
#endif