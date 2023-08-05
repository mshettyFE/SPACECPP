#ifndef DENSITY
#define DENSITY

#include "Bunch.h"
#include "Constants.h"

#include <vector>
#include <limits>
#include <tuple>
#include "Constants.h"

class Density{
  private:
    double lower_bound_tau = std::numeric_limits<double>::infinity();
    double upper_bound_tau = -1.0*lower_bound_tau;
    int nbins;
    Coords density_coordinate;
  public:
    // members
    std::vector<std::tuple<double,double>> discrete_data;
    //functions
    Density(Bunch b, Coords coordinate, int ngrid_pts);
    double MomentGeneratorDiscrete(int moment_number);
  private:
    int getBinNumber(double value);
};

#endif