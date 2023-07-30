#include "Gaussian.h"
#include "Parameters.h"
#include "Constants.h"

#include <iostream>
#include <string>

Gaussian::Gaussian(Parameters gauss_paras, Parameters glob_paras){
  double lower_temp, upper_temp, mu, sigma;
  bool parameter_check = gauss_paras.get_parameter("mu", mu) && gauss_paras.get_parameter("sigma", sigma) && gauss_paras.get_parameter("lower", lower_temp) && gauss_paras.get_parameter("upper", upper_temp);
  if(!parameter_check){
    throw std::runtime_error("Couldn't create gaussian distribution");
  }
  func_paras.add_parameter("mu",std::to_string(mu),DOUBLE);
  func_paras.add_parameter("sigma",std::to_string(sigma),DOUBLE);
  if(upper_temp <= lower_temp){
    throw std::runtime_error("Upper bound is less than lower bound of gaussian");  
  }
  func_paras.add_parameter("lower",std::to_string(lower_temp),DOUBLE);
  func_paras.add_parameter("upper",std::to_string(upper_temp),DOUBLE);
}

double Gaussian::get_lower(){
  double lower_bound;
  func_paras.get_parameter("lower", lower_bound);
  return lower_bound;
}

double Gaussian::get_upper(){
  double upper_bound;
  func_paras.get_parameter("upper", upper_bound);
  return upper_bound;
}

double Gaussian::get_mu(){
  double temp;
  func_paras.get_parameter("mu", temp);
  return temp;
}
double Gaussian::get_sigma(){
  double temp;
  func_paras.get_parameter("sigma", temp);
  return temp;
}

double Gaussian::draw(double x, Parameters glob_paras){
// return unnormalized gaussian
  double mu, sigma;
  func_paras.get_parameter("mu", mu);
  func_paras.get_parameter("sigma", sigma);
  double factor = (x-mu)/sigma;
  return exp(-0.5*factor*factor);
}