#ifndef QUARTIC
#define QUARTIC
#include "MathFunction.h"
#include "Parameters.h"

// Derived class implementing EQ. density with quartic potential

class Quartic : public MathFunction {
  public:
    Quartic(Parameters func_paras, Parameters glob_paras = Parameters());
    double draw(double x, Parameters glob_paras = Parameters()) override;
    double get_lower() override;
    double get_upper() override;
};

#endif