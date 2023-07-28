#ifndef PASSIVE_CAVITY
#define PASSIVE_CAVITY
#include "Cavity.h"
#include "Parameters.h"
#include <unordered_map>
#include <string>

class PassiveCavity: public Cavity{
  private:
    double unloaded_shunt_impedance; // shunt_impedance of cavity
    double unloaded_quality_factor; // quality factor of cavity
    double detune_freq; // detuning frequncy (ie. how off from resonance you are in Hz)
  public:
    PassiveCavity(std::string cav_name, double unloaded_shunt, double unloaded_qual_f, double det_freq, int CavityOrder, double beta);
    // Need to verify ParameterMap when you load it from file (see FileParser.h)
    double Voltage(double tau, const Bunch bunch, Parameters Para) override;
    // print parameters of cavity
    void print() override;

};
#endif