#include "Density.h"
#include "Bunch.h"
#include "Constants.h"
#include "Particle.h"

#include <vector>
#include <tuple>
#include <iostream>

Density::Density(Bunch b, Coords coordinate, int bins){
// bins is the number of bins in the histogram. Must be at least 1
  if(bins <=0){
    throw(std::runtime_error("Invalid number of bins in Density"));
  }
// assign bins and coordinate to Density
  nbins = bins;
  density_coordinate = coordinate;
// create an empty buffer initialized to 0
  std::vector<double> value_buffer;
  for(int i=0; i< nbins; ++i){
    value_buffer.push_back(0);
  }
// find min and maximum  tau values of density
  for(uint64_t part=0; part< b.sim_parts.size(); ++part){
    double current_tau = b.sim_parts[part].getTau();
    if(current_tau >= upper_bound_tau){
      upper_bound_tau = current_tau;
    }
    if(current_tau <= lower_bound_tau){
      lower_bound_tau = current_tau;
    }
  }
// calculate center of bins of histogram
  std::vector<double> center_tau_buffer;
  double dif = (upper_bound_tau-lower_bound_tau)/ (double) (nbins);
  double current_tau = lower_bound_tau+dif/2.0;
  for(int i=0; i< nbins; ++i){
    center_tau_buffer.push_back(current_tau);
    current_tau += dif;
  }  

// run through particles and populate density histogram
  for(uint64_t part=0; part< b.sim_parts.size(); ++part){
    double current_tau, weight;
    current_tau =  b.sim_parts[part].getTau();
// get weight based on which coordinate we care about
    switch(coordinate){
        case TAU:
          weight = 1;
          break;
        case DELTA:
          weight = b.sim_parts[part].getDelta();
          break;
        case X_TRANS:
          weight = b.sim_parts[part].getXTrans();
          break;
        case PX_TRANS:
          weight = b.sim_parts[part].getPXTrans();
          break;
        case Y_TRANS:
          weight = b.sim_parts[part].getYTrans();
          break;
        case PY_TRANS:
          weight = b.sim_parts[part].getPYTrans();
          break;
        default:
          throw std::runtime_error("Invalid coordinate");
          break;
    }
    int index = getBinNumber(current_tau);
// if the particle if out of range, ignore it and move onto next particle
    if(index ==-1){
      continue;
    }
    value_buffer[index] += weight;
  }
// fill up final data structure
  for(int i=0; i< nbins; ++i){
    std::tuple<double,double> pair = std::make_tuple(center_tau_buffer[i],value_buffer[i]);
    discrete_data.push_back(pair);
  }
}

int Density::getBinNumber(double val){
// utility function to map val to bin number
  if(val > upper_bound_tau){
    return -1;
  }
  if(val < lower_bound_tau){
    return -1;
  }
// normal histogram intervals exclude the upper bound. For the rightmost bin, I'm going to include the upper bound
  if(val==upper_bound_tau){
    return nbins-1;
  }
// calculate index where particle falls into
  double dif = (upper_bound_tau-lower_bound_tau)/ (double) (nbins);
  int index = (int) (val-lower_bound_tau)/dif;
  return index;
}

double Density::MomentGeneratorDiscrete(int moment_number){
// Calculate nth moment of the density. Nearly identical to Bunch implementation
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
// calculating mean (ie. 1st moment). formula for weighted mean is (1/\Sigma_{i} w_i)*\Sigma_{i=0} w_i*x_{i}
  long double tally = 0.0;
  long double sum_of_weights= 0.0;
  double weight, cur_tau;
  for(int i=0; i< discrete_data.size(); ++i){
      auto pair = discrete_data[i];
      weight = std::get<1>(pair);
      cur_tau = std::get<0>(pair);
      tally += cur_tau*weight;
      sum_of_weights += weight;
  }
  double mean = tally/ sum_of_weights;
  if(moment_number == 1){
  // 1st normalized moment is just the mean
    moment = mean;
//    std::cout << coordinate << '\t' << moment_number << '\t' << moment << std::endl;
    return moment;
  }
    
  // for higher order moments, we use the formula (1/\Sigma_{i} w_i)*\Sigma_{i=0} w_i(x_{i}-c)^n
  // where N is the number of particles, c is the mean, and n is the moment number
  // n=2 corresponds to variance
  tally = 0.0;
  sum_of_weights= 0.0;
  long double dif = 0.0;
  for(int i=0; i< discrete_data.size(); ++i){
    auto pair = discrete_data[i];
    weight = std::get<1>(pair);
    cur_tau = std::get<0>(pair);
    sum_of_weights += weight;
    dif = cur_tau-mean;
    tally += weight*pow(dif,moment_number);
  }
  moment = tally/sum_of_weights ;
  return moment;
}