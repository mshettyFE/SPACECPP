#ifndef MATH_FUNCTION
#define MATH_FUNCTION

#include "Parameters.h"

class MathFunction{
  protected:
    Parameters func_paras;
  public:
    virtual double draw(double x, Parameters glob_paras = Parameters())=0;
    virtual double get_lower()=0;
    virtual double get_upper()=0;
};

#endif