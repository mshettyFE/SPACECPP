#include "Bunch.h"
#include "Particle.h"
#include "Constants.h"
#include "ProbDist.h"
#include "Parameters.h"

#include <vector>
#include <random>
#include <memory>
#include <unordered_map>
#include <tuple>
#include <iostream>
#include <cmath>
#include  <cstdint>
#include <functional>

#include "doctest.h"

Bunch::Bunch(uint64_t nparticles, std::unordered_map<Coords, std::shared_ptr<ProbDist>> function_map, Parameters GlobalParas){
    if(nparticles <0){
      std::runtime_error("Bunch instantiated with fewer than 0. Need at least 0 (0 corresponds to empty rf bucket");
    }
    bunch_id = ++bunch_id_generator;
    
    std::vector<std::unique_ptr<ProbDist>> cavities;
    
    for(uint64_t i=0; i<nparticles; ++i){
      double tau_roll, delta_roll, x_trans_roll, px_trans_roll;
      tau_roll =  accept_reject(function_map[TAU]);
      delta_roll =  accept_reject(function_map[DELTA]);
      x_trans_roll =  accept_reject(function_map[X_TRANS]);
      px_trans_roll =  accept_reject(function_map[PX_TRANS]);
      sim_parts.push_back(Particle(tau_roll,delta_roll,x_trans_roll, px_trans_roll ));
    }
}

void Bunch::print() const {
    std::cout << bunch_id << "\n";
    for(auto p : sim_parts){
      std::cout << '\t';
      p.print();
    }
}

double Bunch::MomentGeneratorDiscrete(Coords coordinate, int moment_number) const{
// Calculate nth moment of a coordinate on the bunch
  double moment = 0.0;
  if(moment_number < 0){
    std::runtime_error("Invalid moment number (must be at least 0)");
  }
  if(moment_number == 0){
// normalized 0 moment of any distribution is just 1.
    moment = 1.0;
//    std::cout << coordinate << '\t' << moment_number << '\t' << moment << std::endl;
    return moment;
  }
// calculating mean (ie. 1st moment)
  double tally = 0.0;
  for(uint64_t i=0; i<sim_parts.size(); ++i){
    Particle p = sim_parts[i];
    switch (coordinate){
        case TAU:
            tally += p.getTau();
            break;
        case DELTA:
            tally += p.getDelta();
            break;
        case X_TRANS:
            tally += p.getXTrans();
            break;
        case PX_TRANS:
            tally += p.getPXTrans();
            break;
        default:
            std::runtime_error("Invalid coordinate for moment generation");
            break;
    }
  }
  double mean = tally/ (double) sim_parts.size();
  if(moment_number == 1){
  // 1st normalized moment is just the mean
    moment = mean;
//    std::cout << coordinate << '\t' << moment_number << '\t' << moment << std::endl;
    return moment;
  }
    
  // for higher order moments, we use the formula (1/N)*\Sigma_{i=0} (x_{i}-c)^n
  // where N is the number of particles, c is the mean, and n is the moment number
  // n=2 corresponds to variance
  tally = 0.0;
  double dif = 0.0;
  for(uint64_t i=0; i<sim_parts.size(); ++i){
    Particle p = sim_parts[i];
    switch (coordinate){
        case TAU:
            dif = p.getTau()-mean;
            break;
        case DELTA:
            dif = p.getDelta()-mean;
            break;
        case X_TRANS:
            dif = p.getXTrans()-mean;
            break;
        case PX_TRANS:
            dif = p.getPXTrans()-mean;
            break;
        default:
            std::runtime_error("Invalid coordinate for moment generation");
            break;
    }
    tally += pow(dif,moment_number);
  }
  moment = tally/(double) sim_parts.size();
//  std::cout << coordinate << '\t' << moment_number << '\t' << moment << std::endl;
  return moment;
}

double Bunch::MomentGeneratorTau(int moment_number) const{
  double v  = MomentGeneratorDiscrete(TAU, moment_number);
  return v;
}
double Bunch::MomentGeneratorDelta(int moment_number) const {
  double v  = MomentGeneratorDiscrete(DELTA, moment_number);
  return v;
}
double Bunch::MomentGeneratorXTrans(int moment_number) const {
  double v  = MomentGeneratorDiscrete(X_TRANS, moment_number);
  return v;
}
double Bunch::MomentGeneratorPXTrans(int moment_number) const {
  double v  = MomentGeneratorDiscrete(PX_TRANS, moment_number);
  return v;
}

/*
TEST_CASE("Testing MomentGeneratorDiscrete and wrapper functions...") {
    std::unordered_map<Coords, std::tuple<double,double>> coord_parameters;
    std::tuple<double, double> tau = std::make_tuple(-5,2.6);
    coord_parameters[TAU] = tau;
    std::tuple<double, double> delta = std::make_tuple(0.6,1.2);
    coord_parameters[DELTA] = delta;
    std::tuple<double, double> xt = std::make_tuple(0,1);
    coord_parameters[X_TRANS] = xt;
    std::tuple<double, double> pxt = std::make_tuple(23,75);
    coord_parameters[PX_TRANS] = pxt;
    // Generate 1 million particles to test
    Bunch Bchs(1000000,coord_parameters);
    double epsilon = 1E-1;
    double true_val, calc;
    // TAU
    true_val = 1;
    calc = Bchs.MomentGeneratorTau(0);
    CHECK( ( abs(calc-true_val) < epsilon) == true);
    true_val = -5;
    calc = Bchs.MomentGeneratorTau(1);
    CHECK( ( abs(calc-true_val) < epsilon) == true);
    true_val = 2.6;
    calc = Bchs.MomentGeneratorTau(2);
    CHECK( ( abs(sqrt(calc)-true_val) < epsilon) == true);

    //DELTA
    true_val = 0.6;
    calc = Bchs.MomentGeneratorDelta(1);
    CHECK( ( abs(calc-true_val) < epsilon) == true);
    true_val = 1.2;
    calc = Bchs.MomentGeneratorDelta(2);
    CHECK( ( abs(sqrt(calc)-true_val) < epsilon) == true);

    //X_TRANS
    true_val = 0;
    calc = Bchs.MomentGeneratorXTrans(1);
    CHECK( ( abs(calc-true_val) < epsilon) == true);
    true_val = 1;
    calc = Bchs.MomentGeneratorXTrans(2);
    CHECK( ( abs(sqrt(calc)-true_val) < epsilon) == true);
    
    //PX_TRANS
    true_val = 23;
    calc = Bchs.MomentGeneratorPXTrans(1);
    CHECK( ( abs(calc-true_val) < epsilon) == true);
    true_val = 75;
    calc = Bchs.MomentGeneratorPXTrans(2);
    CHECK( ( abs(sqrt(calc)-true_val) < epsilon) == true);
}
*/

double Bunch::accept_reject(std::shared_ptr<ProbDist> initial_dist, Parameters GlobalParas, double max_tries){
  static thread_local std::mt19937 generator;
  std::uniform_real_distribution<double> uni_dist_x(initial_dist->get_min_x(), initial_dist->get_max_x());
  std::uniform_real_distribution<double> uni_dist_y(0.0,initial_dist->get_max_y());
  for(int i=0; i<max_tries; ++i){
    double x = uni_dist_x(generator);
    double p1 =  initial_dist->draw(x,GlobalParas);
    double p2 = uni_dist_y(generator);
    if(p2<= p1){
      return x;
    }
  }
  std::runtime_error("Exceeded max tries to draw from distribution");
  return 0.0;
}

uint64_t Bunch::bunch_id_generator = 0;