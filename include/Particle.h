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
    // Arrival time of particle w.r.t. reference particle (tau=0 for a reference particle)
    double tau;
    // momentum deviation from reference particle (3 Gev for NSLS-II)
    double delta;
    // Transverse x momentum
    double x_trans;
    // Transverse p momentum
    double px_trans;
    // Transverse y momentum
    double y_trans;
    // Transverse py momentum
    double py_trans;
  public:
    // Constructors
    Particle(double tau,double delta,double x,double px, double y, double py);
    Particle();
    // update coorinates of particle
    void update(double t,double del,double x,double px, double y, double py);
    // set individual coordinate of particle
    void setTau(double t);
    void setDelta(double del);
    void setXTrans(double x);
    void setPXTrans(double px);
    void setYTrans(double y);
    void setPYTrans(double py);
    // Extract coordinates from particle
    double getTau();
    double getDelta();
    double getXTrans();
    double getPXTrans();
    double getYTrans();
    double getPYTrans();
    std::tuple<double,double,double,double,double,double> getCoordinates();
    // printing
    void print();
};
#endif