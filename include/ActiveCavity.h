#ifndef ACTIVE_CAVITY
#define ACTIVE_CAVITY
#include "Cavity.h"
#include <iostream>
#include <unordered_map>
#include <string>

class ActiveCavity: public Cavity{
  private:
    double r; // relative voltage
    double Phase;
    double BeamLoadingAngle;
  public:
    ActiveCavity(std::string cav_name, double relative_voltage, double CavityPhase, double BeamLoadingAngle, int CavityOrder, double beta);
    // Voltage has this signature to be consistant with PassiveCavity class
    // Need to verify ParameterMap when you load it from file (see FileParser.h)
    double Voltage(double tau, const Bunch bunch, Parameters Para) override;
    // print parameters of cavity
    void print() override;
};
#endif
