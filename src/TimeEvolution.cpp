#include "TimeEvolution.h"
#include "Bunch.h"
#include "Cavity.h"
#include "Parameters.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>

#ifdef MPI_FOUND
#include <mpi.h>
#endif

TimeEvolution::TimeEvolution(std::vector<std::unique_ptr<Cavity>>& cavities, std::vector<Bunch>& bunches, Parameters& GlobalParas){
  for (int i=0; i< cavities.size(); ++i){
      Cavities.push_back(std::move(cavities[i]));
  }
    for(auto b : bunches){
      Bunches.push_back(b);    
    }
    GlobParas = GlobalParas;
    double loss_per_turn, Vrf;
    bool check = GlobParas.get_parameter("Uloss", loss_per_turn) && GlobParas.get_parameter("vrf", Vrf);
    if(!check){
      throw std::runtime_error("Couldn't read Uloss or vrf");
    }
    relative_loss = loss_per_turn/Vrf;
}

double TimeEvolution::Voltage(double tau, int bunch_index){
  double sum = -relative_loss;
  double E0, T0;
  bool check = GlobParas.get_parameter("E0",E0) && GlobParas.get_parameter("T0",T0);
  for(const auto &cav: Cavities){
    sum += cav->Voltage(tau, Bunches[bunch_index], GlobParas);
  }
  return sum/E0/T0;
}

double TimeEvolution::integrate(int bunch_index, double lower, double upper, int steps){
  double f_low = Voltage(lower,bunch_index);
  double f_high = Voltage(upper,bunch_index);
  double sum = 0.5*(f_low+f_high);
  double delta = (upper-lower)/(double) steps;
  for(int i =1 ;i < steps; ++i){
    double x = lower + delta*i;
    double cur = Voltage(x,bunch_index);
    sum += cur;
  }
  return sum*delta;
}

double TimeEvolution::Potential(double tau, int bunch_index, int steps){
    if( (bunch_index >= Bunches.size()) ||(bunch_index <0) ){
        throw std::runtime_error("Bunch index out of range");
    }
    double min_tau = Bunches[bunch_index].get_min_tau();
    if(tau< min_tau){
      throw std::runtime_error("tau less than lower bound in Potential function call");
    }
    if(steps <=0){
      throw std::runtime_error("Number of steps must be at least 1 in Potential function call");    
    }
    return integrate(bunch_index, min_tau, tau, steps);
}

void TimeEvolution::PlotPotential(std::string fname, int bunch_index, double lower_bound, double upper_bound,  int steps, int sub_steps){
  if( (bunch_index >= Bunches.size()) ||(bunch_index <0) ){
      throw std::runtime_error("Bunch index out of range");
  }
  if(upper_bound< lower_bound){
    throw std::runtime_error("upper bound less than lower bound in PlotPotential function call");
  }
    if(steps <=0){
      throw std::runtime_error("Number of steps must be at least 1 in PlotPotential function call");    
    }
    if(sub_steps <=0){
      throw std::runtime_error("Number of sub_steps must be at least 1 in PlotPotential function call");    
    }
    
  std::ofstream file;
  file.open(fname);
  file << "Tau,Potential" << std::endl;
  double cur_lower = lower_bound;
  double delta = (upper_bound-lower_bound)/(double) steps;
  double sum = 0.0;
  double cur = 0.0;
  for(int i =1 ;i < steps; ++i){
    double cur_upper = lower_bound+i*delta;
    cur = integrate(bunch_index, cur_lower, cur_upper, sub_steps);
    cur_lower = cur_upper;
    sum += cur;
    file <<cur_upper << ',' << sum << std::endl;
  }
  file.close();
}

void TimeEvolution::PlotVoltage(std::string fname, int bunch_index, double lower_bound, double upper_bound, int steps){
  if( (bunch_index >= Bunches.size()) ||(bunch_index <0) ){
      throw std::runtime_error("Bunch index out of range");
  }
  if(upper_bound< lower_bound){
    throw std::runtime_error("upper bound less than lower bound in PlotPotential function call");
  }
    if(steps <=0){
      throw std::runtime_error("Number of steps must be at least 1 in Potential function call");    
    }    
  std::ofstream file;
  file.open(fname);
  file << "Tau,Voltage" << std::endl;
  double cur=0;
  double delta = (upper_bound-lower_bound)/(double) steps;
  for(int i =1 ;i < steps; ++i){
    double current_x = lower_bound+i*delta;
    cur =  Voltage(current_x, bunch_index);
    file <<current_x << ',' << cur << std::endl;
  }
  file.close();
}

/*
void TimeEvolution::update(){
  for(int i=0; i< Bunches.size(); ++i){
    Hamiltonia
  }
}

void TimeEvolution::HamiltonianUpdate(Bunch& bunch){
  
}
*/