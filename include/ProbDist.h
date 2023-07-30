#ifndef PROB_DIST
#define PROB_DIST
#include <unordered_map>
#include <string>
#include <random>

#include "MathFunction.h"
#include "Gaussian.h"
#include "Parameters.h"

class ProbDist{
  private:
    double maximum;
    double normalization;
    MathFunction* f;
  public:
    ProbDist(MathFunction* func, Parameters GlobalParas = Parameters(), double tol = 1e-8, int steps=1000, int max_iterations = 1000);
    double get_maximum();
    double get_norm();
    double get_lower();
    double get_upper();
    double draw(double x, Parameters glob_paras = Parameters());
    ~ProbDist();
  private:
    bool derive_normalization(double& output, Parameters GlobalParas, int steps);
    bool find_a_max(double& output, Parameters GlobalParas, double tol, int max_iterations );
};
#endif