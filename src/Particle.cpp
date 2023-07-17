#include "Particle.h"
#include <tuple>

// Constructors
Particle::Particle(double delta_in, double tau_in, double x_trans_in, double p_trans_in){
      delta = delta_in;
      tau = tau_in;
      x_trans = x_trans_in;
      p_trans = p_trans_in;
    };
Particle::Particle(){
      delta = 0.0;
      tau = 0.0;
      x_trans = 0.0;
      p_trans = 0.0;
    };

// Update all at once
void Particle::update(double delta_in, double tau_in, double x_trans_in, double p_trans_in){
      delta = delta_in;
      tau = tau_in;
      x_trans = x_trans_in;
      p_trans = p_trans_in;
};

// getter methods
double Particle::getDelta(){return delta;};
double Particle::getTau(){return tau;};
double Particle::getXTrans(){return x_trans;};
double Particle::getPTrans(){return p_trans;};
std::tuple<double,double,double,double> Particle::getCoordinates(){
  std::tuple<double,double,double,double>values = std::make_tuple(delta,tau,x_trans,p_trans);
  return values;
}
