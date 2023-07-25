#include <cmath>
#include <unordered_map>
#include <string>

#include "Constants.h"
#include "UniformDist.h"
#include "Parameters.h"

double UniformDist::func(double x,Parameters globalParas){
  return 1.0/(max_x-min_x);
}
