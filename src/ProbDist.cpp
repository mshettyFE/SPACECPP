#include "MathFunction.h"
#include "ProbDist.h"
#include "Quartic.h"
#include "Parameters.h"

#include <iostream>

#include "doctest.h"

// NOTE: You don't need to delete *func by hand since ProbDist will automatically take care of this for you

ProbDist::ProbDist(MathFunction* func, Parameters GlobalParas, double tol, int steps, int max_iterations){
  f = func;
  bool  par_check = (tol > 0) && (steps  >0) && (max_iterations  >0);
  if(!par_check){
    throw std::runtime_error("Couldn't read tolerance, steps and/or max_iterations");
  }
  bool validate_norm = derive_normalization(normalization, GlobalParas, steps);
  bool validate_max  = find_a_max(maximum, GlobalParas, tol, max_iterations);
  if(!(validate_norm && validate_max)){
    throw std::runtime_error("Couldn't create probability distribution");
  }
}

ProbDist::~ProbDist(){
  delete f;
}

double ProbDist::get_norm(){
  return normalization;
}

double ProbDist::get_maximum(){
  return maximum;
}

double ProbDist::get_lower(){
  return f->get_lower();
}

double ProbDist::get_upper(){
  return f->get_upper();
}

double ProbDist::draw(double x, Parameters glob_paras){
  return f->draw(x,glob_paras)/normalization;
}

bool ProbDist::derive_normalization(double& output, Parameters GlobalParas, int steps){
    double lower = get_lower();
    double upper = get_upper();
    double f_low = f->draw(lower, GlobalParas);
    double f_high = f->draw(upper, GlobalParas);
    if((f_low <0) || (f_high<0)){
        std::cerr << "invalid endpoints" << std::endl;
        return false;
    }
    double sum = 0.5*(f_low+f_high);
    double delta = (upper-lower)/(double) steps;
    for(int i =1 ;i < steps; ++i){
      double x = lower + delta*i;
      double cur = f->draw(x, GlobalParas);
      if(cur < 0){
        std::cerr << "invalid probability distribution" << std::endl;
        return false;      
      }
      sum += cur;
    }
  output =  sum*(delta);
  return true;
}

bool ProbDist::find_a_max(double& output, Parameters GlobalParas, double tol, int max_iterations ){
// Follows Golden Section algorithm from numerical recipes (adapted for maximum finder. (ie. I only changed (f_left < f_right) to (f_left > f_right))
// assumes that there is a single maximum on the interval (ie. like a gaussian)
  double up_x = get_upper();
  double low_x = get_lower();
  const double R = (sqrt(5) - 1) / 2.0;
  const double C = 1.0-R;
  double right_x = up_x*R + low_x*C;
  double left_x = low_x*R + up_x*C;
  double f_left;
  double f_right;
  int count = 0;
  while(abs(up_x-low_x) > tol){
    if(count > max_iterations){
      return false;
    }
    count++;
    f_left = draw(left_x,GlobalParas);
    f_right = draw(right_x,GlobalParas);
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
  output = draw((up_x+low_x)/2.0, GlobalParas);
  return true;
}

TEST_CASE("Quartic generation"){
  Parameters FunctionParas =  Parameters();
  FunctionParas.add_parameter("constant","0.5",DOUBLE);
  FunctionParas.add_parameter("upper","10",DOUBLE);
  FunctionParas.add_parameter("lower","-10",DOUBLE);
  Quartic* q = new Quartic(FunctionParas);
  ProbDist p = ProbDist(q);
  CHECK( (abs(p.get_norm()-2.155800549540928) < 1E-4) == true);
}

TEST_CASE("Gaussian generation"){
  Parameters FunctionParas =  Parameters();
  double mu, sigma;
  mu = 0.5;
  sigma = 1;
  FunctionParas.add_parameter("mu",std::to_string(mu),DOUBLE);
  FunctionParas.add_parameter("sigma",std::to_string(sigma), DOUBLE);
  FunctionParas.add_parameter("upper",std::to_string(mu+5*sigma),DOUBLE);
  FunctionParas.add_parameter("lower",std::to_string(mu-5*sigma),DOUBLE);
  Gaussian* q = new Gaussian(FunctionParas);
  ProbDist p = ProbDist(q);
  CHECK( (abs(p.get_norm()-1.772453850905516) < 1E-4) == true);
}