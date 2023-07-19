#include "Bunch.h"
#include "Particle.h"
#include "Constants.h"
#include <vector>
#include <random>
#include <unordered_map>
#include <tuple>
#include <iostream>
#include <cmath>
#include  <cstdint>
#include <functional>

#include "doctest.h"

Bunch::Bunch(std::unordered_map<std::string,std::string> ParameterMap, std::unordered_map<Coords, std::tuple<double,double>> coord_parameters){
    double v=  std::stod(ParameterMap["npop"]);
    int nparticles = static_cast<int>(v);
    if(nparticles <=0){
      std::runtime_error("Bunch instantiated with fewer than less than 0");
    }
    bunch_id = ++bunch_id_generator;
    // seed and define gaussian rngs for each coordinate
    static thread_local std::mt19937 generator;
    auto tau_tup = coord_parameters[TAU];
    std::normal_distribution<double> tau_dist(std::get<0>(tau_tup),std::get<1>(tau_tup));
    auto delta_tup = coord_parameters[DELTA];
    std::normal_distribution<double> delta_dist(std::get<0>(delta_tup),std::get<1>(delta_tup));
    auto x_trans_tup = coord_parameters[X_TRANS];
    std::normal_distribution<double> x_trans_dist(std::get<0>(x_trans_tup),std::get<1>(x_trans_tup));
    auto px_trans_tup = coord_parameters[PX_TRANS];
    std::normal_distribution<double> px_trans_dist(std::get<0>(px_trans_tup),std::get<1>(px_trans_tup));
    
    double tau_roll, delta_roll, x_trans_roll, px_trans_roll;
    for(uint64_t i=0; i<nparticles; ++i){
      tau_roll = tau_dist(generator);
      delta_roll = delta_dist(generator);
      x_trans_roll = x_trans_dist(generator);
      px_trans_roll = px_trans_dist(generator);
      sim_parts.push_back(Particle(tau_roll,delta_roll,x_trans_roll, px_trans_roll ));
    }
}

Bunch::Bunch(uint64_t nparticles, std::unordered_map<Coords, std::tuple<double,double>> coord_parameters){
    if(nparticles <=0){
      std::runtime_error("Bunch instantiated with fewer than less than 0. Need more than 0");
    }
    bunch_id = ++bunch_id_generator;
    // seed and define gaussian rngs for each coordinate
    static thread_local std::mt19937 generator;
    auto tau_tup = coord_parameters[TAU];
    std::normal_distribution<double> tau_dist(std::get<0>(tau_tup),std::get<1>(tau_tup));
    auto delta_tup = coord_parameters[DELTA];
    std::normal_distribution<double> delta_dist(std::get<0>(delta_tup),std::get<1>(delta_tup));
    auto x_trans_tup = coord_parameters[X_TRANS];
    std::normal_distribution<double> x_trans_dist(std::get<0>(x_trans_tup),std::get<1>(x_trans_tup));
    auto px_trans_tup = coord_parameters[PX_TRANS];
    std::normal_distribution<double> px_trans_dist(std::get<0>(px_trans_tup),std::get<1>(px_trans_tup));
    
    double tau_roll, delta_roll, x_trans_roll, px_trans_roll;
    for(uint64_t i=0; i<nparticles; ++i){
      tau_roll = tau_dist(generator);
      delta_roll = delta_dist(generator);
      x_trans_roll = x_trans_dist(generator);
      px_trans_roll = px_trans_dist(generator);
      sim_parts.push_back(Particle(tau_roll,delta_roll,x_trans_roll, px_trans_roll ));
    }
}

void Bunch::print(){
    std::cout << bunch_id << "\n";
    for(auto p : sim_parts){
      std::cout << '\t';
      p.print();
    }
}

double Bunch::MomentGeneratorDiscrete(Coords coordinate, int moment_number){
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

double Bunch::MomentGeneratorTau(int moment_number){
  double v  = MomentGeneratorDiscrete(TAU, moment_number);
  return v;
}
double Bunch::MomentGeneratorDelta(int moment_number){
  double v  = MomentGeneratorDiscrete(DELTA, moment_number);
  return v;
}
double Bunch::MomentGeneratorXTrans(int moment_number){
  double v  = MomentGeneratorDiscrete(X_TRANS, moment_number);
  return v;
}
double Bunch::MomentGeneratorPXTrans(int moment_number){
  double v  = MomentGeneratorDiscrete(PX_TRANS, moment_number);
  return v;
}

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

uint64_t Bunch::bunch_id_generator = 0;