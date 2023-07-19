#ifndef BUNCH
#define BUNCH
#include "Particle.h"
#include "Constants.h"
#include <vector>
#include <random>
#include <unordered_map>
#include <tuple>

//class to encapsulate a number of particles and define methods on this list of particles

class Bunch{
  private:
    uint64_t bunch_id;
    static uint64_t bunch_id_generator;
    std::vector<Particle> sim_parts; // array of particles assigned to the bunch
// Public functions
  public:
    Bunch(std::unordered_map<std::string,std::string> ParameterMap, std::unordered_map<Coords, std::tuple<double,double>> coord_parameters);
    Bunch(uint64_t nparticles, std::unordered_map<Coords, std::tuple<double,double>> coord_parameters);
    double MomentGeneratorTau(int moment_number);
    double MomentGeneratorDelta(int moment_number);
    double MomentGeneratorXTrans(int moment_number);
    double MomentGeneratorPXTrans(int moment_number);
    void print();
// Private functions
  private:
    double MomentGeneratorDiscrete(Coords coordinate, int moment_number);

};
#endif