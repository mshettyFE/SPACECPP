#ifndef PROB_DIST
#define PROB_DIST
#include <unordered_map>
#include <string>
#include <random>

#include "MathFunction.h"
#include "Gaussian.h"
#include "Parameters.h"

// Class that coverts a MathFunction into a valid probability density. Assumes unimodal distribution
// This could be extended to non unimodal functions via sweeping MathFunction's domain and setting maximum to the largest found value, but that isn't efficient and we only really care about unimodal initial distributions for now

class ProbDist{
  private:
    double maximum; // largest value of function
    double normalization; // normalization constant
    MathFunction* f; // Math function to be turned into probability distribution
  public:
    ProbDist(MathFunction* func, Parameters GlobalParas = Parameters(), double tol = 1e-8, int steps=1000, int max_iterations = 1000);
    double get_maximum();
    double get_norm();
    double get_lower();
    double get_upper();
// Get value of x where we interpret f as a probability distribution
    double draw(double x, Parameters glob_paras = Parameters());
    ~ProbDist();
  private:
// calculates normalization via trapezoid rule
    bool derive_normalization(double& output, Parameters GlobalParas, int steps);
// Finds maximum via golden section (hence unimodal assumption)
    bool find_a_max(double& output, Parameters GlobalParas, double tol, int max_iterations );
};
#endif