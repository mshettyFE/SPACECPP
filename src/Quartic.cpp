#include "Quartic.h"
#include "Parameters.h"
#include "Constants.h"

#include <iostream>
#include <string>

Quartic::Quartic(Parameters function_paras, Parameters glob_paras){
  double lower_temp, upper_temp, constant;
  bool parameter_check = function_paras.get_parameter("constant", constant) && function_paras.get_parameter("lower", lower_temp) && function_paras.get_parameter("upper", upper_temp);
  if(!parameter_check){
    throw std::runtime_error("Couldn't create gaussian distribution");
  }
  if(upper_temp <= lower_temp){
    throw std::runtime_error("Upper bound is less than lower bound of gaussian");  
  }
  func_paras.add_parameter("lower",lower_temp,DOUBLE);
  func_paras.add_parameter("upper",upper_temp,DOUBLE);
  func_paras.add_parameter("constant",constant,DOUBLE);

}

double Quartic::get_lower(){
  double lower_bound;
  func_paras.get_parameter("lower", lower_bound);
  return lower_bound;
}

double Quartic::get_upper(){
  double upper_bound;
  func_paras.get_parameter("upper", upper_bound);
  return upper_bound;
}

double Quartic::draw(double x, Parameters glob_paras){
// return unormalized distribution of the form exp(-con*x^4)
  double con = func_paras.get_parameter("constant",con);
  return exp(-con*x*x*x*x);
}