#include "Bunch.h"
#include "Particle.h"
#include "Constants.h"
#include "Parameters.h"
#include "Gaussian.h"
#include "Quartic.h"
#include "ProbDist.h"

#include <vector>
#include <fstream>
#include <random>
#include <memory>
#include <unordered_map>
#include <tuple>
#include <iostream>
#include <cmath>
#include  <cstdint>
#include <functional>
#include <chrono>

#include "doctest.h"

Bunch::Bunch(uint64_t nparticles, std::unordered_map<Coords, std::unique_ptr<ProbDist>>& function_map, Parameters GlobalParas){
    if(nparticles <0){
      throw std::runtime_error("Bunch instantiated with fewer than 0. Need at least 0 (0 corresponds to empty rf bucket");
    }
    bunch_id = ++bunch_id_generator;
// for each particle, perform basic accept_reject method to generate arbitrary distribution along a coordinate according to some derived class of ProbDist.h
    for(uint64_t i=0; i<nparticles; ++i){
      double tau_roll, delta_roll, x_trans_roll, px_trans_roll, y_trans_roll, py_trans_roll;
      tau_roll =  accept_reject(function_map[TAU]);
      assign_min_max(tau_roll, min_tau,max_tau);
        
      delta_roll =  accept_reject(function_map[DELTA]);
      assign_min_max(delta_roll, min_delta,max_delta);

      x_trans_roll =  accept_reject(function_map[X_TRANS]);
      assign_min_max(x_trans_roll, min_x_trans,max_x_trans);
      
      px_trans_roll =  accept_reject(function_map[PX_TRANS]);
      assign_min_max(px_trans_roll, min_px_trans,max_px_trans);

      y_trans_roll =  accept_reject(function_map[Y_TRANS]);
      assign_min_max(y_trans_roll, min_y_trans,max_y_trans);
      
      py_trans_roll =  accept_reject(function_map[PY_TRANS]);
      assign_min_max(py_trans_roll, min_py_trans,max_py_trans);

      sim_parts.push_back(Particle(tau_roll,delta_roll,x_trans_roll, px_trans_roll, y_trans_roll, py_trans_roll ));
    }
}

void Bunch::print_particles() const {
    std::cout << bunch_id << "\n";
    for(auto p : sim_parts){
      std::cout << '\t';
      p.print();
    }
}

void Bunch::print() const {
    std::cout << bunch_id << "\n";
    std::cout << "Tau\t" << min_tau << '\t' << max_tau << std::endl;
    std::cout << "Delta\t" << min_delta << '\t' << max_delta << std::endl;
    std::cout << "XTrans\t" << min_x_trans << '\t' << max_x_trans << std::endl;
    std::cout << "PXTrans\t" << min_px_trans << '\t' << max_px_trans << std::endl;
    std::cout << "YTrans\t" << min_y_trans << '\t' << max_y_trans << std::endl;
    std::cout << "PYTrans\t" << min_py_trans << '\t' << max_py_trans << std::endl;
}

void Bunch::write_data(std::string fname){
// write phase space coordinate data to basic csv file
  std::ofstream myfile;
  myfile.open (fname);
  myfile << "Tau,Delta,X,PX,Y,Py" << std::endl;
  for(auto p : sim_parts){
    myfile << p.getTau() <<',' << p.getDelta() << ',' << p.getXTrans() << ',' << p.getPXTrans() << ',' << p.getYTrans() << ',' << p.getPYTrans() <<  std::endl;
  }
  myfile.close();
}


double Bunch::MomentGeneratorDiscrete(Coords coordinate, int moment_number) const{
// Calculate nth moment of a coordinate on the bunch
  double moment = 0.0;
  if(moment_number < 0){
    throw std::runtime_error("Invalid moment number (must be at least 0)");
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
        case Y_TRANS:
            tally += p.getYTrans();
            break;
        case PY_TRANS:
            tally += p.getPYTrans();
            break;
        default:
            throw std::runtime_error("Invalid coordinate for moment generation");
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
        case Y_TRANS:
            dif = p.getYTrans()-mean;
            break;
        case PY_TRANS:
            dif = p.getPYTrans()-mean;
            break;
        default:
            throw std::runtime_error("Invalid coordinate for moment generation");
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

double Bunch::MomentGeneratorYTrans(int moment_number) const {
  double v  = MomentGeneratorDiscrete(Y_TRANS, moment_number);
  return v;
}

double Bunch::MomentGeneratorPYTrans(int moment_number) const {
  double v  = MomentGeneratorDiscrete(PY_TRANS, moment_number);
  return v;
}

TEST_CASE("Testing MomentGeneratorDiscrete and wrapper functions...") {
    std::unordered_map<Coords, std::unique_ptr<ProbDist>> func_map;
    
    Parameters func_para = Parameters();
    double mu = -5;
    double sigma = 2.6;
    func_para.add_parameter("mu",mu,  DOUBLE);
    func_para.add_parameter("sigma",sigma,  DOUBLE);
    func_para.add_parameter("lower",mu-5*sigma, DOUBLE);
    func_para.add_parameter("upper",mu+5*sigma, DOUBLE);
    Gaussian* g1 =  new Gaussian(func_para);
    func_map[TAU] = std::make_unique<ProbDist>(g1);

    func_para = Parameters();
    mu = 0.6;
    sigma = 1.2;
    func_para.add_parameter("mu",mu,  DOUBLE);
    func_para.add_parameter("sigma",sigma,  DOUBLE);
    func_para.add_parameter("lower",mu-5*sigma, DOUBLE);
    func_para.add_parameter("upper",mu+5*sigma, DOUBLE);
    Gaussian* g2 = new Gaussian(func_para);
    func_map[DELTA] = std::make_unique<ProbDist>(g2);
    
    func_para = Parameters();
    mu  = 0;
    sigma = 1;
    func_para.add_parameter("mu",mu,  DOUBLE);
    func_para.add_parameter("sigma",sigma,  DOUBLE);
    func_para.add_parameter("lower",mu-5*sigma, DOUBLE);
    func_para.add_parameter("upper",mu+5*sigma, DOUBLE);
    Gaussian* g3 =  new Gaussian(func_para);
    func_map[X_TRANS] = std::make_unique<ProbDist>(g3);
    
    func_para = Parameters();
    mu = 23.0;
    sigma = 75.0;
    func_para.add_parameter("mu",mu,  DOUBLE);
    func_para.add_parameter("sigma",sigma,  DOUBLE);
    func_para.add_parameter("lower",mu-5*sigma, DOUBLE);
    func_para.add_parameter("upper",mu+5*sigma, DOUBLE);
    Gaussian* g4 =  new Gaussian(func_para);
    func_map[PX_TRANS] = std::make_unique<ProbDist>(g4);

    func_para = Parameters();
    mu  = 10;
    sigma = 20;
    func_para.add_parameter("mu",mu,  DOUBLE);
    func_para.add_parameter("sigma",sigma,  DOUBLE);
    func_para.add_parameter("lower",mu-5*sigma, DOUBLE);
    func_para.add_parameter("upper",mu+5*sigma, DOUBLE);
    Gaussian* g5 =  new Gaussian(func_para);
    func_map[Y_TRANS] = std::make_unique<ProbDist>(g5);
    
    func_para = Parameters();
    mu = 50.0;
    sigma = 42.0;
    func_para.add_parameter("mu",mu,  DOUBLE);
    func_para.add_parameter("sigma",sigma,  DOUBLE);
    func_para.add_parameter("lower",mu-5*sigma, DOUBLE);
    func_para.add_parameter("upper",mu+5*sigma, DOUBLE);
    Gaussian* g6 =  new Gaussian(func_para);
    func_map[PY_TRANS] = std::make_unique<ProbDist>(g6);

    Bunch Bchs(100000, func_map);
    Bchs.write_data("TestBunch.csv");
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

    //Y_TRANS
    true_val = 10;
    calc = Bchs.MomentGeneratorYTrans(1);
    CHECK( ( abs(calc-true_val) < epsilon) == true);
    true_val = 20;
    calc = Bchs.MomentGeneratorYTrans(2);
    CHECK( ( abs(sqrt(calc)-true_val) < epsilon) == true);
    
    //PY_TRANS
    true_val = 50;
    calc = Bchs.MomentGeneratorPYTrans(1);
    CHECK( ( abs(calc-true_val) < epsilon) == true);
    true_val = 42;
    calc = Bchs.MomentGeneratorPYTrans(2);
    CHECK( ( abs(sqrt(calc)-true_val) < epsilon) == true);
}

double Bunch::accept_reject(std::unique_ptr<ProbDist>& initial_dist, Parameters GlobalParas, int max_tries){
// Accept reject is a simple monte carlo method to sample any arbitrary pdf
// Imagine a rectange that encompasses your pdf. The rectange has width = upper_x-lower_x and a height that is equal to the absolute maximum of the pdf
// you randomly pick a point within this rectange. if this point lies above the desired pdf, you reject the point and re-roll. If it is below the pdf, then you accept the point and exit.
// If you select enough random points, you can approximate the desired pdf to an arbitrary precision.
// This implementation assumes that the function is univariate (ie. there is only 1 local maximum of the pdf)
// TODO: If you wanted to extend this to handle multivariate initial distribution, you would need to modify get_a_max function of your derived ProbDist class appropriately (one way to do this is to randomly select points on your interval, find the local maxima of each of these points, and then poll the max of these maximum. I didn't do this because the functions I care about for initial distributions are univariate (guassians, quartics etc).
  static thread_local std::mt19937 generator;
  auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
  generator.seed((unsigned long)seed);
  std::uniform_real_distribution<double> uni_dist_x(initial_dist->get_lower(), initial_dist->get_upper());
  std::uniform_real_distribution<double> uni_dist_y(0.0,initial_dist->get_maximum());
  for(int i=0; i<max_tries; ++i){
    double x = uni_dist_x(generator);
    double p1 =  initial_dist->draw(x,GlobalParas);
    double p2 = uni_dist_y(generator);
    if(p2<= p1){
      return x;
    }
  }
  throw std::runtime_error("Exceeded max tries to draw from distribution");
  return 0.0;
}

void Bunch::assign_min_max(double candidate, double& min, double& max){
  if(candidate < min){
    min = candidate;
  }
  if(candidate > max){
    max = candidate;
  }
}

double Bunch::get_min_tau(){
  return min_tau;
}

double Bunch::get_max_tau(){
  return max_tau;
}

uint64_t Bunch::bunch_id_generator = 0;