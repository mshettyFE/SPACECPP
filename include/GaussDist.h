#ifndef GAUSS_DIST
#define GAUSS_DIST
#include "ProbDist.h"
#include "Parameters.h"

class GaussDist: public ProbDist{
  private:
    double mu, sigma; // mean and std. dev of distribution
  public:
    // Generates pdf function to be used in accept reject method
    GaussDist(Parameters func_p, Parameters glob_para =Parameters(),double tol=1e-6, int max_steps_find_max=1000, int n_steps_integrate=1000); 
    // extracts probability of x occuring according to this gaussian distribution
    double draw(double x, Parameters globalParas = Parameters()) override; 

    double get_norm();
    double get_max();
    double get_mu();
    double get_sigma();
    double get_lower_x() override;
    double get_upper_x() override;

  protected:
    // internal function used to draw non-normalized gaussian
    double draw_unnormalized(double x, Parameters globalParas = Parameters()) override;
    // Locates a maximum of the distribution via golden section algorithm
    bool find_a_max(double& output, Parameters globalParas = Parameters(), double precision = 1e-8, int max_iters = 100) override;
    // basic trapezoidal integration to determine normalization constant
    bool integrate(double lower, double upper, int steps, double& output, Parameters glob_para = Parameters()) override;
};

#endif