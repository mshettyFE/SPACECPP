#ifndef CAVITY
#define CAVITY

#include <unordered_map>
#include <string>

#include "Bunch.h"
// Simple abstract class to define an rf cavity.
// Cavity defines an rf cavity. each cavity is uniquely defined by some voltage which is a function of tau
// For the passive cavity, we need to calculate sigma_t (2nd moment) to gerate the current profile to generate the voltage, hence why Bunch object is passed as well
class Cavity{
  protected:
    std::string name; // name of cavity
    double Phase; // phase of cavity
    double order; // order of cavity (1 for main, greater than 1 for hhcs)
  public:
    virtual double Voltage(double tau,const Bunch bunch, const std::unordered_map<std::string,std::string> ParameterMap) = 0;
    virtual void print() = 0;
};
#endif
