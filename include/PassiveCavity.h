#ifndef PASSIVE_CAVITY
#define PASSIVE_CAVITY
#include "Cavity.h"
#include <unordered_map>
#include <string>

class PassiveCavity: public Cavity{
  private:
    double shunt_impedance; // shunt_impedance of cavity
  public:
    PassiveCavity(std::string cav_name, double shunt_impedance, double Phase, double order);
    // Need to verify ParameterMap when you load it from file (see FileParser.h)
    double Voltage(double tau, const Bunch bunch, const std::unordered_map<std::string,std::string> ParameterMap) override;
    // print parameters of cavity
    void print() override;

};
#endif