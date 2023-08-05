#ifndef TIME_EVOLUTION_MPI
#define TIME_EVOLUTION_MPI
#include "Bunch.h"
#include "Cavity.h"
#include "Parameters.h"

#include <vector>
#include <string>
#include <random>

class TimeEvolution{
  private:
    std::vector<std::unique_ptr<Cavity>> Cavities;
    std::vector<Bunch> Bunches;
    Parameters GlobParas;
    double relative_loss;
    std::mt19937 generator;
  public:
    TimeEvolution(std::vector<std::unique_ptr<Cavity>>& cavities, std::vector<Bunch>& in_bunches, Parameters& GlobalParas);
    double Voltage(double tau, int bunch_index);
    double Potential(double min_tau, double max_tau, int bunch_index, int steps);
    void PlotPotential(std::string fname, int bunch_index, double lower_x, double upper_x, int steps=100, int sub_steps=100);
    void PlotVoltage(std::string fname, int bunch_index, double lower_x, double upper_x,  int steps=100);
    void run_simulation(bool HamiltonianFlag=1, bool FPFlag=0, bool WakefieldFlag=0, bool verbose=0);
  private:
    void update(int turn_number, bool HamiltonianFlag=1, bool FPFlag=1, bool WakefieldFlag=0);
    void HamiltonianUpdate(int bunch_index);
    void FPUpdate(int bunch_index);
    double integrate(int bunch_index, double lower, double upper, int steps=100);
};

#endif