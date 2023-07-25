#ifndef UNIFORM_DIST
#define UNIFORM_DIST
#include "ProbDist.h"
#include "Parameters.h"

class UniformDist: public ProbDist{
      using ProbDist::ProbDist;
      double func(double x,Parameters globalParas) override;
};

#endif