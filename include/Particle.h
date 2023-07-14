#ifndef PARTICLE
#define PARTICLE
#include<tuple>
// Class that defines a simulation particle
class Particle{
  private:
    double delta;
    double tau;
    double x_trans;
    double p_trans;
  public:
    Particle(double,double,double,double);
    Particle();
    void update(double,double,double,double);
    double getDelta();
    double getTau();
    double getXTrans();
    double getPTrans();
    std::tuple<double,double,double,double> getCoordinates();
};
#endif