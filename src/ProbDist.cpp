#include "ProbDist.h"
#include "Parameters.h"
#include <unordered_map>
#include <string>
#include <random>
#include <iostream>

ProbDist::ProbDist(Parameters FuncParams, double min_x_candidate, double max_x_candidate, Parameters globalParas){
  min_x = min_x_candidate;
  max_x = max_x_candidate;
  if(max_x <= min_x){
    std::runtime_error("Couldn't initialize Probability distribution. max_x is smaller than min_x");  
  }
  func_parameters = FuncParams;
  bool norm_check = normalize(normalization, globalParas);
  bool max_check = find_max_y(max_y, globalParas);
  if(!(max_check && norm_check)){
    std::runtime_error("Couldn't initialize Probability distribution");
  }  
}

double ProbDist::draw(double x, Parameters globalParas){
    return func(x,globalParas)/normalization;
}

bool ProbDist::normalize(double &output_norm, Parameters globalParas){
  int x_steps = 10000;
  double sum = 0.0;
  double step = (max_x-min_x)/ (double) x_steps;
  for(int i=0; i< x_steps; ++i){
    double x_pos = min_x+i*step;
    double height = func(x_pos, globalParas);
    if(height <0){
      std::cerr << "Invalid probability distribution" << std::endl;
      return false;
    }
    sum += height;
  }
  if(sum <0){
      std::cerr << "Invalid probability distribution. Sum is negative" << std::endl;
      return false;
  }
  output_norm = sum*step;
  return true;
}

bool ProbDist::find_max_y(double &output_max, Parameters globalParas, double precision, int max_iters){
// Follows Golden Section algorithm from numerical methods (adapted for maximum finder)
// assumes that there is a single local maximum on the interval (ie. like a gaussian)
    const double tol = sqrt(precision);
    const double R = 0.61803399;
    const double C=1.0-R; // Golden ratio
    double dx, x0,x1,x2,x3;
    x0 = min_x;
    x3 = max_x;
    dx = x3-x0;
    x1 = (dx)/2.0;
    x2 = x1+C*(dx);
    double f1,f2;
    int count = 0;
    while(abs(dx) > tol*(abs(x1)+abs(x2))){
      if(count >= max_iters){
        return false;
      }
      else{
        count+= 1;
      }
      f1= draw(x1, globalParas);
      f2= draw(x2, globalParas);
      if(f2 > f1){
        x1 = x2;
        x2 = R*x2+C*x3;
      }
      else{
        x2 = x1;
        x1 = R*x1+C*x0;
      }
    }
    if(f1 < f2){
      output_max= f2;
      return true;
    }
    output_max  = f1;
    return true;
}

double ProbDist::get_max_y(){
  return max_y;
}
double ProbDist::get_max_x(){
  return max_x;
}
double ProbDist::get_min_x(){
  return min_x;
}
