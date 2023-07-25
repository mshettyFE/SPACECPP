#include <cmath>
#include <unordered_map>
#include <string>

#include "Constants.h"
#include "GaussDist.h"
#include "Parameters.h"

double GaussDist::func(double x,Parameters globalParas){
  double mu, sigma;
  bool mu_check = func_parameters.get_parameter("mu",mu);
  bool sigma_check = func_parameters.get_parameter("sigma",sigma);
  if(!(mu_check && sigma_check)){
    std::runtime_error("Invalid mu and sigma");
  }
  double factor = (x-mu)/sigma;
  return exp(-0.5*factor*factor)/sigma/sqrt(2*pi);
}
