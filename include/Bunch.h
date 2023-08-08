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
#include <limits>

#include "cereal/types/vector.hpp"

//class to encapsulate a number of particles and define methods on this list of particles

class Bunch{
  private:
    uint64_t bunch_id;
    int nRealPerSim;
    static uint64_t bunch_id_generator; // Used to automatically assign bunch_id. Bunch id can be reassigned
  public:
    std::vector<Particle> sim_parts; // array of particles assigned to the bunch
// Public functions
  public:
    Bunch();
    Bunch(uint64_t nparticles, int nRealParticlesPerSim, std::unordered_map<Coords, std::unique_ptr<ProbDist>>& function_map, Parameters GlobalParas= Parameters());
// calculte Nth moment along each coordinate
    double MomentGeneratorTau(int moment_number) const ;
    double MomentGeneratorDelta(int moment_number) const ;
    double MomentGeneratorXTrans(int moment_number) const ;
    double MomentGeneratorPXTrans(int moment_number) const ;
    double MomentGeneratorYTrans(int moment_number) const ;
    double MomentGeneratorPYTrans(int moment_number) const ;
// getter and setter for bunch id
    uint64_t get_id();
    void set_id(uint64_t new_id);
// print() displays bunch paramters
    void print() const ;
// print_particles() displays coordinates of particles in bunch
    void print_particles() const ;
// write particle coordinates to csv file
    void write_data(std::string fname);
// serialization. see See https://stackoverflow.com/questions/61743418/im-getting-a-strange-error-when-i-try-to-do-serialization-with-cereal-in-c for why implementation is in header file
// serialization is used to let me copy Bunches between processors with OpenMPI
    template<class Archive>
    void serialize(Archive & archive)
    {
      archive(bunch_id, nRealPerSim, CEREAL_NVP(sim_parts));
    }
  private:
// function that gets called by other MomentGenerator functions. Actually calculates moments
    double MomentGeneratorDiscrete(Coords coordinate, int moment_number) const ;
    // performs accept reject algorithm on arbitrary probability distribution (ie. normalized function)
    double accept_reject(std::unique_ptr<ProbDist>& initial_dist, Parameters GlobalParas = Parameters(),  int max_tries=1000);
};
#endif