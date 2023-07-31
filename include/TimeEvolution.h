#ifndef TIME_EVOLUTION_MPI
#define TIME_EVOLUTION_MPI
#include "Bunch.h"
#include "Cavity.h"
#include "Parameters.h"

#include <vector>
#include <string>

class TimeEvolution{
  private:
    std::vector<std::unique_ptr<Cavity>> Cavities;
    std::vector<Bunch> Bunches;
    Parameters GlobParas;
    double relative_loss;
  public:
    TimeEvolution(std::vector<std::unique_ptr<Cavity>>& cavities, std::vector<Bunch>& bunches, Parameters& GlobalParas);
//    void update();
    double Voltage(double tau, int bunch_index);
    double Potential(double tau, int bunch_index, int steps);
    void PlotPotential(std::string fname, int bunch_index, double lower_x, double upper_x, int steps=100, int sub_steps=100);
    void PlotVoltage(std::string fname, int bunch_index, double lower_x, double upper_x,  int steps=100);
  private:
//    void HamiltonianUpdate();
    double integrate(int bunch_index, double lower, double upper, int steps=100);
};

#endif