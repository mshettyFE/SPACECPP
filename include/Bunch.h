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
    static uint64_t bunch_id_generator;
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
    double min_y_trans = p_inf;
    double max_y_trans = n_inf;
    double min_py_trans = p_inf;
    double max_py_trans = n_inf;
  public:
    std::vector<Particle> sim_parts; // array of particles assigned to the bunch
// Public functions
  public:
    Bunch(uint64_t nparticles, std::unordered_map<Coords, std::unique_ptr<ProbDist>>& function_map, Parameters GlobalParas= Parameters());
    double MomentGeneratorTau(int moment_number) const ;
    double MomentGeneratorDelta(int moment_number) const ;
    double MomentGeneratorXTrans(int moment_number) const ;
    double MomentGeneratorPXTrans(int moment_number) const ;
    double MomentGeneratorYTrans(int moment_number) const ;
    double MomentGeneratorPYTrans(int moment_number) const ;
    void print() const ;
    void print_particles() const ;
    double get_min_tau();
    double get_max_tau();
    void HamiltonianUpdate(Parameters GlobalParas);
    void FPUpdate(Parameters GlobalParas);
    void write_data(std::string fname);
// serialization. see See https://stackoverflow.com/questions/61743418/im-getting-a-strange-error-when-i-try-to-do-serialization-with-cereal-in-c for why implementation is in header file
    template <class Archive>
    void save(Archive & archive) const
    {
      archive(bunch_id, CEREAL_NVP(sim_parts));
    }
    template <class Archive>
    void load(Archive & archive)
    {
      archive(bunch_id, p_inf,
    n_inf ,
    min_tau ,
    max_tau ,
    min_delta ,
    max_delta ,
    min_x_trans ,
    max_x_trans ,
    min_px_trans ,
    max_px_trans ,
    min_y_trans ,
    max_y_trans ,
    min_py_trans ,
    max_py_trans , CEREAL_NVP(sim_parts));
    }

// Private functions
  private:
    double MomentGeneratorDiscrete(Coords coordinate, int moment_number) const ;
    // performs accept reject algorithm on arbitrary probability distribution (ie. normalized function)
    double accept_reject(std::unique_ptr<ProbDist>& initial_dist, Parameters GlobalParas = Parameters(),  int max_tries=1000);
    void assign_min_max(double candidate, double& min, double& max);
};
#endif