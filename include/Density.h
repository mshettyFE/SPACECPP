#ifndef DENSITY
#define DENSITY

#include "Bunch.h"
#include "Constants.h"

#include <vector>
#include <limits>
#include <tuple>
#include "Constants.h"

// Function to model various densities derived from Bunch data as a histogram
// Defined at middle of bins
class Density{
  private:
// lower and upper bounds of distribution for the histogram
    double lower_bound_tau = std::numeric_limits<double>::infinity();
    double upper_bound_tau = -1.0*lower_bound_tau;
// number of bins of histogram
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