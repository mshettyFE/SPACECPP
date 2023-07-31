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

//class to encapsulate a number of particles and define methods on this list of particles

class Bunch{
  private:
    uint64_t bunch_id;
    static uint64_t bunch_id_generator;
    std::vector<Particle> sim_parts; // array of particles assigned to the bunch
    double p_inf = std::numeric_limits<double>::infinity();
    double n_inf = -1.0*p_inf;
    double min_tau = p_inf;
    double max_tau = n_inf;
    double min_delta = p_inf;
    double max_delta = n_inf;
    double min_x_trans = p_inf;
    double max_x_trans = n_inf;
    double min_px_trans = p_inf;
    double max_px_trans = n_inf;
// Public functions
  public:
    Bunch(uint64_t nparticles, std::unordered_map<Coords, std::unique_ptr<ProbDist>>& function_map, Parameters GlobalParas= Parameters());
    double MomentGeneratorTau(int moment_number) const ;
    double MomentGeneratorDelta(int moment_number) const ;
    double MomentGeneratorXTrans(int moment_number) const ;
    double MomentGeneratorPXTrans(int moment_number) const ;
    void print() const ;
    void print_particles() const ;
    double get_min_tau();
    double get_max_tau();
    void write_data(std::string fname);
// Private functions
  private:
    double MomentGeneratorDiscrete(Coords coordinate, int moment_number) const ;
    // performs accept reject algorithm on arbitrary probability distribution (ie. normalized function)
    double accept_reject(std::unique_ptr<ProbDist>& initial_dist, Parameters GlobalParas = Parameters(),  int max_tries=1000);
    void assign_min_max(double candidate, double& min, double& max);
};
#endif