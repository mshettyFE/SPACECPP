#ifndef TIME_EVOLUTION_MPI
#define TIME_EVOLUTION_MPI
#include "Bunch.h"
#include "Cavity.h"
#include "Parameters.h"

#include <vector>
#include <string>
#include <random>

// class that bunches Cavity data, bunch data, and input parameter data and performs symplectic updates to particles

class TimeEvolution{
  private:
    std::vector<std::unique_ptr<Cavity>> Cavities;
    std::vector<Bunch> Bunches;
    std::string preappend; // preappended string that is added to Bunch data output file name. Used to specify run number
    Parameters GlobParas; // Global parameters
    double relative_loss; // Energy loss per  turn normalized to main cavity voltage
    std::mt19937 generator; // RNG for quantum fluctuations
  public:
    TimeEvolution(std::vector<std::unique_ptr<Cavity>>& cavities, std::vector<Bunch>& in_bunches, Parameters& GlobalParas, std::string add_to_front="");
// calculate voltage and potential energy
    double Voltage(double tau, int bunch_index);
    double Potential(double min_tau, double max_tau, int bunch_index, int steps);
// Plotting functions to sanity check voltage and potential. Also to check progress
    void PlotPotential(std::string fname, int bunch_index, double lower_x, double upper_x, int steps=100, int sub_steps=100);
    void PlotVoltage(std::string fname, int bunch_index, double lower_x, double upper_x,  int steps=100);
// The main function of the class. Runs simulation for nturns
    void run_simulation(bool HamiltonianFlag=1, bool FPFlag=0, bool WakefieldFlag=0, bool verbose=0);
  private:
// Wrapper function around Hamiltonian and FP update
    void update(int turn_number, bool HamiltonianFlag=1, bool FPFlag=1, bool WakefieldFlag=0);
    void HamiltonianUpdate(int bunch_index);
    void FPUpdate(int bunch_index);
// helper function that calculates definite integral with trapezoid rule
    double integrate(int bunch_index, double lower, double upper, int steps=100);
};

#endif