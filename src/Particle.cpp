#include "Particle.h"
#include <tuple>
#include <iostream>

// Constructors
Particle::Particle(double tau_in, double delta_in,  double x_trans_in, double p_trans_in){
      part_id = part_id_generator++;
      tau = tau_in;
      delta = delta_in;
      x_trans = x_trans_in;
      px_trans = p_trans_in;
    };
Particle::Particle(){
      part_id = part_id_generator++;
      tau = 0.0;
      delta = 0.0;
      x_trans = 0.0;
      px_trans = 0.0;
    };

// Update all at once
void Particle::update(double tau_in, double delta_in, double x_trans_in, double px_trans_in){
      tau = tau_in;
      delta = delta_in;
      x_trans = x_trans_in;
      px_trans = px_trans_in;
};

// getter methods
double Particle::getDelta(){return delta;};
double Particle::getTau(){return tau;};
double Particle::getXTrans(){return x_trans;};
double Particle::getPXTrans(){return px_trans;};
std::tuple<double,double,double,double> Particle::getCoordinates(){
  std::tuple<double,double,double,double>values = std::make_tuple(delta,tau,x_trans,px_trans);
  return values;
}
// Printing
void Particle::print(){
      std::cout << part_id << '\t' << tau << '\t' << delta << '\t' << x_trans << '\t' << px_trans << std::endl;
}

uint64_t Particle::part_id_generator = 0;
