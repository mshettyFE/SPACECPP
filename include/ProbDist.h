#ifndef PROB_DIST
#define PROB_DIST
#include <unordered_map>
#include <string>
#include <random>

#include "Parameters.h"

class ProbDist{
  private:
  public:
    ProbDist(Parameters FuncParams, double min_x, double max_x, Parameters globalParas = Parameters());
    double draw(double x, Parameters globalParas  = Parameters());
    double get_max_y();
    double get_max_x();
    double get_min_x();
  protected:
    double min_x;
    double max_x;
    double max_y;
    double normalization;
    Parameters func_parameters;
    virtual double func(double x, Parameters globalParas  = Parameters() ) = 0;
  private:
bool find_max_y(double &output_max, Parameters globalParas, double precision = 1E-8, int max_iters = 10000);
    bool normalize(double &output_norm, Parameters globalParas = Parameters());
};
#endif