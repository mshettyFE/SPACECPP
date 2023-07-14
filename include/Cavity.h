#ifndef CAVITY
#define CAVITY
#include "Bunch.h"
// Simple abstract class to define an rf cavity.
// Cavity defines an rf cavity. each cavity is uniquely defined by some voltage which is a function of tau
// For the passive cavity, we need to calculate sigma_t (2nd moment) to gerate the current profile to generate the voltage, hence why Bunch object is passed as well
class Cavity{
  private:
    double Phase; // phase of cavity
    double order; // order of cavity (1 for main, greater than 1 for hhcs)
  virtual double Voltage(double tau, Bunch bunch) = 0;
};
#endif