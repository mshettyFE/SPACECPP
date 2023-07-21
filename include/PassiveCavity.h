#ifndef PASSIVE_CAVITY
#define PASSIVE_CAVITY
#include "Cavity.h"
#include <unordered_map>
#include <string>

class PassiveCavity: public Cavity{
  private:
    double shunt_impedance; // shunt_impedance of cavity
    double quality_factor;
    double detune_freq;
    double Phi;
  public:
    PassiveCavity(std::string cav_name, int n_harm, double frf, double shunt, double qual_f, double det_freq, int CavityOrder);
    // Need to verify ParameterMap when you load it from file (see FileParser.h)
    double Voltage(double tau, const Bunch bunch) override;
    // print parameters of cavity
    void print() override;

};
#endif