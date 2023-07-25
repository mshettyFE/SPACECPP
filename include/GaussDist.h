#ifndef GAUSS_DIST
#define GAUSS_DIST
#include "ProbDist.h"

class GaussDist: public ProbDist{
      using ProbDist::ProbDist;
      double func(double x,Parameters globalParas) override;
};

#endif