#ifndef PROB_DIST
#define PROB_DIST
#include <unordered_map>
#include <string>
#include <random>

#include "Parameters.h"

// Abstract base class for probability distributions
// This feels like a clunky way of doing this, since the only thing that should change between pdfs if what function you are drawing from. However, you can't have virtual constructors, and I had a difficult time thinking of a way to do this in a better manner.
// So for now, you need to override all the virtual methods whenever you need to define a new distribution

class ProbDist{
  private:
  public:
    virtual double draw(double x, Parameters globalParas = Parameters()) = 0;
    virtual double get_norm() = 0;
    virtual double get_max() = 0;
    virtual double get_lower_x() = 0;
    virtual double get_upper_x() = 0;
  protected:
    Parameters func_parameters;
    double normalization;
    double maximum;
    double lower_x;
    double upper_x;
    virtual double draw_unnormalized(double x, Parameters globalParas = Parameters()) = 0;
    virtual bool find_a_max(double& output, Parameters globalParas = Parameters(), double precision = 1e-8, int max_iters = 1000) = 0;
    virtual bool integrate(double lower, double upper, int steps, double& output, Parameters glob_para = Parameters()) = 0;
};
#endif