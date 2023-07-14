#ifndef PASSIVE_CAVITY
#define PASSIVE_CAVITY
#include "Cavity.h"
#include <unordered_map>
#include <string>

class PassiveCavity: public Cavity{
  private:
    double shunt_impedance; // shunt_impedance of cavity
  public:
    PassiveCavity(double shunt_impedance, double Phase, double order, std::unordered_map<std::string,double>);
    double Voltage(double tau, Bunch bunch);
};
#endif