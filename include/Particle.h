#ifndef PARTICLE
#define PARTICLE
#include<tuple>
// Class that defines a simulation particle
class Particle{
  private:
    // momentum deviation from reference particle (3 Gev for NSLS-II)
    double delta;
    // Arrival time of particle w.r.t. reference particle (tau=0 for a reference particle)
    double tau;
    // Transverse x momentum
    double x_trans;
    // Transverse p momentum
    double p_trans;
  public:
    // Constructors
    Particle(double del,double t,double x,double p);
    Particle();
    // update coorinates of particle
    void update(double del,double t,double x,double p);
    // Extract coordinates from particle
    double getDelta();
    double getTau();
    double getXTrans();
    double getPTrans();
    std::tuple<double,double,double,double> getCoordinates();
};
#endif