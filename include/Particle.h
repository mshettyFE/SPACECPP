#ifndef PARTICLE
#define PARTICLE
#include<tuple>
#include <cstdint>

// Class that defines a simulation particle
// Basically a glorified struct at the moment
class Particle{
  private:
    uint64_t part_id;
    static uint64_t part_id_generator;
    // momentum deviation from reference particle (3 Gev for NSLS-II)
    double delta;
    // Arrival time of particle w.r.t. reference particle (tau=0 for a reference particle)
    double tau;
    // Transverse x momentum
    double x_trans;
    // Transverse p momentum
    double px_trans;
  public:
    // Constructors
    Particle(double del,double t,double x,double px);
    Particle();
    // update coorinates of particle
    void update(double del,double t,double x,double px);
    // Extract coordinates from particle
    double getDelta();
    double getTau();
    double getXTrans();
    double getPXTrans();
    std::tuple<double,double,double,double> getCoordinates();
    // printing
    void print();
};
#endif