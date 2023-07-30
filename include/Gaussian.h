#ifndef GAUSSIAN
#define GAUSSIAN
#include "MathFunction.h"
#include "Parameters.h"

class Gaussian : public MathFunction {
  public:
    Gaussian(Parameters gauss_paras, Parameters glob_paras = Parameters());
    double draw(double x, Parameters glob_paras = Parameters()) override;
    double get_lower() override;
    double get_upper() override;
    double get_mu();
    double get_sigma();
};

#endif