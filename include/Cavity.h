#ifndef CAVITY
#define CAVITY

#include <unordered_map>
#include <string>

#include "Constants.h"
#include "Bunch.h"
#include "Parameters.h"
// Simple abstract class to define an rf cavity.
// Cavity defines an rf cavity. each cavity is uniquely defined by some voltage which is a function of tau
// For the passive cavity, we need to calculate sigma_t (2nd moment) to gerate the current profile to generate the voltage, hence why Bunch object is passed as well
class Cavity{
  protected:
    // name and order assigned to each cavity
    std::string name; // name of cavity
    CavityType cav_type; // cavity type
    double beta; // beta coupling
    int order; // order of cavity (1 for main, greater than 1 for hhcs)
  public:
    // Need to verify ParameterMap when you load it from file (see FileParser.h)
    // Need to override voltage and print with another class (probably need to do this when doing beam-loading compensation (ie. active-passive cavity)?)
    virtual double Voltage(double tau,const Bunch bunch, Parameters Para) = 0;
    // print out parameters of the cavity
    virtual void print() = 0;
};
#endif
