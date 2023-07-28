#ifndef BUNCH
#define BUNCH
#include "Particle.h"
#include "ProbDist.h"
#include "Constants.h"
#include <vector>
#include <random>
#include <memory>
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
    Bunch(uint64_t nparticles, std::unordered_map<Coords, std::unique_ptr<ProbDist>>& function_map, Parameters GlobalParas= Parameters());
    double MomentGeneratorTau(int moment_number) const ;
    double MomentGeneratorDelta(int moment_number) const ;
    double MomentGeneratorXTrans(int moment_number) const ;
    double MomentGeneratorPXTrans(int moment_number) const ;
    void print() const ;
    void write_data(std::string fname);
// Private functions
  private:
    double MomentGeneratorDiscrete(Coords coordinate, int moment_number) const ;
    // performs accept reject algorithm on arbitrary probability distribution (ie. normalized function)
    double accept_reject(std::unique_ptr<ProbDist>& initial_dist, Parameters GlobalParas = Parameters(),  int max_tries=1000);
};
#endif