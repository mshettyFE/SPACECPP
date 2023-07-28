#include <cmath>
#include <unordered_map>
#include <string>
#include <iostream>
#include <stdexcept>

#include "doctest.h"

#include "Constants.h"
#include "GaussDist.h"
#include "Parameters.h"

GaussDist::GaussDist(Parameters func_p, Parameters glob_para,double tol, int max_steps_find_max, int n_steps_integrate){
// verify that mu and sigma exist and are valid
  func_parameters = func_p;
  bool func_para_check = func_p.get_parameter("mu", mu) &&  func_p.get_parameter("sigma", sigma);
  if(!(func_para_check || (sigma <=0)) ){
    throw std::runtime_error("Couldn't initialize Gaussian Distribution. Invalid input parameters");  
  }
// lower and upper bounds exists to enable accept reject method and find_a_max method
// Function holds for all values on R, this was just a workaround to the fact that I need a finite range to use accept reject
  lower_x = mu-5*sigma;
  upper_x = mu+5*sigma;
// normalize gaussian to make it a pdf
  bool normalization_check = integrate(lower_x, upper_x, n_steps_integrate, normalization, glob_para);
  bool max_check = find_a_max(maximum, glob_para, tol,max_steps_find_max);
  if(!(normalization_check && max_check)){
    throw std::runtime_error("Couldn't initialize Gaussian Distribution");
  }
}

double GaussDist::draw(double x, Parameters globalParas){
// Returns value of normalized function
  return draw_unnormalized(x,globalParas)/normalization;
}

double GaussDist::draw_unnormalized(double x, Parameters globalParas){
  double factor = (x-mu)/sigma;
  return exp(-0.5*factor*factor);
}

bool GaussDist::integrate(double lower, double upper, int steps, double& output, Parameters glob_para){
// standard trapezoidal integration (see numerical recipes)
    if(steps<=0){
        std::cerr << "number of steps must be greater than 0" << std::endl;
      return false;
    }
    if(upper < lower){
        std::cerr << "upper bound smaller than lower bound" << std::endl;
      return false;
    }
    double f_low = draw_unnormalized(lower, glob_para);
    double f_high = draw_unnormalized(upper, glob_para);
    if((f_low <0) || (f_high<0)){
        std::cerr << "invalid endpoints" << std::endl;
        return false;
    }
    double sum = 0.5*(f_low+f_high);
    double delta = (upper-lower)/(double) steps;
    for(int i =1 ;i < steps; ++i){
      double x = lower + delta*i;
      double cur = draw_unnormalized(x, glob_para);
      if(cur < 0){
        std::cerr << "invalid probability distribution" << std::endl;
        return false;      
      }
      sum += cur;
    }
  output =  sum*(delta);
  return true;
}

bool GaussDist::find_a_max(double& output, Parameters globalParas, double tol, int max_iters){
// Follows Golden Section algorithm from numerical recipes (adapted for maximum finder. (ie. I only changed (f_left < f_right) to (f_left > f_right))
// assumes that there is a single maximum on the interval (ie. like a gaussian)
  double up_x = upper_x;
  double low_x = lower_x;
  const double R = (sqrt(5) - 1) / 2.0;
  const double C = 1.0-R;
  double right_x = up_x*R + low_x*C;
  double left_x = low_x*R + up_x*C;
  double f_left;
  double f_right;
  int count = 0;
  while(abs(up_x-low_x) > tol){
    if(count > max_iters){
      return false;
    }
    count++;
    f_left = draw(left_x,globalParas);
    f_right = draw(right_x,globalParas);
    double r1, r2;
    if( f_left > f_right){
      up_x = right_x;
      right_x = left_x;
      left_x = R*low_x+C*up_x;
    }
    else{
      low_x = left_x;
      left_x = right_x;
      right_x = C*low_x+R*up_x;
    }
  }
  output = draw((up_x+low_x)/2.0, globalParas);
  return true;
};

// public getter methods
double GaussDist::get_norm(){
  return normalization;
}

double GaussDist::get_max(){
  return maximum;
}

double GaussDist::get_mu(){
  return mu;
}
double GaussDist::get_sigma(){
  return sigma;
}

double GaussDist::get_lower_x(){
  return lower_x;
}
double GaussDist::get_upper_x(){
  return upper_x;
}


TEST_CASE("Testing GaussDist constructor...") {
  Parameters p;
  p.add_parameter("mu","-5",DOUBLE);
  p.add_parameter("sigma","2.6",DOUBLE);
  auto dist = GaussDist(p);
// gives normalization of the form 1/(sigma*sqrt(2*pi))
  CHECK(((dist.get_norm()-3*sqrt(2*pi))<1e-3) == true);
// for a gaussian, the max is at mu, which has a value of 1/normalization
  CHECK(((dist.get_max()-1.0/dist.get_norm())<1e-3)==true);
}
