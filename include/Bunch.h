#ifndef BUNCH
#define BUNCH
#include Particle.h
#include <vector>

//class to encapsulate a number of particles and define methods on this list of particles

class Bunch{
  private:
    std::vector<Particle> sim_parts; // array of particles assigned to the bunch
// Public functions
  public:
    Bunch(int nparticles);
    MomentGeneratorDelta(int moment_number);
    MomentGeneratorTau(int moment_number);
    MomentGeneratorXTrans(int moment_number);
    MomentGeneratorPTrans(int moment_number);
// Private functions
  private:
    double MomentGeneratorDiscrete(int coordinate, int moment_number);
};
#endif