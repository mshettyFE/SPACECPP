#include "Particle.h"
#include <tuple>

// Constructors
Particle::Particle(double delta_in, double tau_in, double x_trans_in, double p_trans_in){
      delta = delta_in;
      tau = tau_in;
      x = x_trans_in;
      p = p_trans_in;
    };
Particle::Particle(){
      delta = 0.0;
      tau = 0.0;
      x = 0.0;
      p = 0.0;
    };

// Update all at once
void Particle::update(double delta_in, double tau_in, double x_trans_in, double p_trans_in){
      delta = delta_in;
      tau = tau_in;
      x = x_trans_in;
      p = p_trans_in;
};

// getter methods
double Particle::getDelta(){return delta};
double Particle::getTau(){return tau};
double Particle::getXTrans(){return x};
double Particle::getPTrans(){return p};
std::tuple<double,double,double,double> Particle::getCoordinates(){
  return std::tuple<double,double,double,double> values(delta,tau,x,p);
}
