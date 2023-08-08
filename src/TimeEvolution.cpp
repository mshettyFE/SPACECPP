#include "TimeEvolution.h"
#include "Bunch.h"
#include "Cavity.h"
#include "Parameters.h"
#include "Constants.h"
#include "MPIHelper.h"

#include <vector>
#include <fstream>
#include <iostream>
#include <string>
#include <chrono>
#include <random>

#include <mpi.h>

TimeEvolution::TimeEvolution(std::vector<std::unique_ptr<Cavity>>& cavities, std::vector<Bunch>& in_bunches, Parameters& GlobalParas, std::string add_to_front){
  preappend = add_to_front;
  int world_size;
  MPI_Comm_size(MPI_COMM_WORLD, &world_size);
// create input_bunch length variable that is either set to 0 or nbunches
  int input_bunch_sizes = in_bunches.size();
// broadcast number of bunches from root to the rest of the processors
  MPI_Barrier(MPI_COMM_WORLD);
  MPI_Bcast(&input_bunch_sizes, 1, MPI_INT, 0, MPI_COMM_WORLD);
  MPI_Barrier(MPI_COMM_WORLD);
  // genreate the intervals on which each processor acts on. This is why we needed the aforementioned broadcast
  std::vector<std::tuple<int,int>> intervals = MPIHelper::generate_intervals(world_size, input_bunch_sizes);
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
// Since root processor has access to all bunches, can directly assign it's own bunches
  if(world_rank==0){
    std::tuple<int,int> range = intervals[0];
    int lower_bound_inclusive = std::get<0>(range);
    int upper_bound_exclusive = std::get<1>(range);
    for(int bunch_index = lower_bound_inclusive; bunch_index < upper_bound_exclusive; ++bunch_index){
      Bunches.push_back(in_bunches[bunch_index]);
    }
  }
// synchronize all processes
  MPI_Barrier(MPI_COMM_WORLD);
  if(world_rank==0){
    for(int i=1; i < world_size; ++i){
      std::tuple<int,int> range = intervals[i];
      int lower_bound_inclusive = std::get<0>(range);
      int upper_bound_exclusive = std::get<1>(range);
// copy over necessary bunches to a buffer
      std::vector<Bunch> bunch_buffer;
      for(int bunch_index = lower_bound_inclusive; bunch_index < upper_bound_exclusive; ++bunch_index){
        bunch_buffer.push_back(in_bunches[bunch_index]);
      }
// serialize buffer data
      std::string serialized_bunch_data = MPIHelper::SerializeData(bunch_buffer);
// send over message to other processor of rank i
      uint64_t msg_length = serialized_bunch_data.length();
      MPI_Send(&msg_length, 1, MPI_UINT64_T, i, 0, MPI_COMM_WORLD);
      MPI_Send(serialized_bunch_data.c_str(), msg_length , MPI_CHAR, i, 0, MPI_COMM_WORLD);
    }
  }
// the other processors receive the bunches assigned by the root processor
  else{
      std::tuple<int,int> range = intervals[world_rank];
      int lower_bound_inclusive = std::get<0>(range);
      int upper_bound_exclusive = std::get<1>(range);
      uint64_t serial_buffer_size;
// Read in incoming message size from root
      MPI_Recv(&serial_buffer_size, 1, MPI_UINT64_T, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
// create character buffer and read to buffer
      char *buf = new char[serial_buffer_size];
      MPI_Recv(buf, serial_buffer_size, MPI_CHAR, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
// convert to string and delete char buf
      std::string recv_serial_data(buf,serial_buffer_size);
      delete [] buf;
// de-serialize data to local Bunches
      MPIHelper::DeserializeData(recv_serial_data,Bunches);
  }

  for (int i=0; i< cavities.size(); ++i){
      Cavities.push_back(std::move(cavities[i]));
  }
    GlobParas = GlobalParas;
    double loss_per_turn, Vrf;
    bool check = GlobParas.get_parameter("Uloss", loss_per_turn) && GlobParas.get_parameter("vrf", Vrf);
    if(!check){
      throw std::runtime_error("Couldn't read Uloss or vrf");
    }
    relative_loss = loss_per_turn/Vrf;
    auto seed = std::chrono::high_resolution_clock::now().time_since_epoch().count();
    generator.seed((unsigned long)seed);
  MPI_Barrier(MPI_COMM_WORLD);
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

double TimeEvolution::Potential(double min_tau, double max_tau, int bunch_index, int steps){
    if( (bunch_index >= Bunches.size()) ||(bunch_index <0) ){
        throw std::runtime_error("Bunch index out of range");
    }
    if(max_tau< min_tau){
      throw std::runtime_error("tau less than lower bound in Potential function call");
    }
    if(steps <=0){
      throw std::runtime_error("Number of steps must be at least 1 in Potential function call");    
    }
    return integrate(bunch_index, min_tau, max_tau, steps);
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

void TimeEvolution::run_simulation(bool HamiltonianFlag, bool FPFlag, bool WakefieldFlag, bool verbose){
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    if(world_rank==0){
      std::cout << "Starting Simulation..." << std::endl;
      std::cout << "Rank\tCurTurn\tTotalTurn\tTiming(ms)" << std::endl;
    }
    using std::chrono::high_resolution_clock;
    using std::chrono::duration;
    using std::chrono::milliseconds;
 if( !(HamiltonianFlag || FPFlag || WakefieldFlag )){
    std::cout << "No update made" << std::endl;
    return;
  }
  int nturns;
  bool check  = GlobParas.get_parameter("nturns",nturns);
  if(!check){
    throw std::runtime_error("Invalid number of turns when attempting to run simulation");
  }
  if(nturns==0){
    throw std::runtime_error("Invalid number of turns when attempting to run simulation");  
  }
  for(int i=0; i<nturns; ++i){
    auto t1 = high_resolution_clock::now();
    update(i,HamiltonianFlag,FPFlag,WakefieldFlag);
    auto t2 = high_resolution_clock::now();
    duration<double, std::milli> ms_double = t2 - t1;
    if(verbose){
      std::cout << world_rank << '\t' << i << '\t' << nturns << '\t' << ms_double.count() <<  std::endl;    
    }
    for(auto b: Bunches){
      std::string fname = data_folder_path+"/"+preappend+"_Bunch_"+std::to_string(b.get_id())+"_turn_"+std::to_string(i)+".csv";
      b.write_data(fname);
    }
  }
}


void TimeEvolution::update(int turn_number, bool HamiltonianFlag,bool FPFlag, bool WakefieldFlag){
  if( !(HamiltonianFlag || FPFlag || WakefieldFlag )){
    std::cout << "No update made" << std::endl;
    return;
  }
  for(int i=0; i< Bunches.size(); ++i){
    if(HamiltonianFlag){
      HamiltonianUpdate(i);
    }
    if(FPFlag){
      FPUpdate(i);
    }
  }
  if(WakefieldFlag){
    std::cout << "Wakefield calculation unimplemented" << std::endl;
    return;
  }
  return ;
}

void TimeEvolution::HamiltonianUpdate(int bunch_index){
// Taken from  the following paper:
/*
G. Bassi, A. Blednykh and V. Smaluk, Phys. Rev. Accel. Beams
19, 024401, 2016.
*/
  double Vrf, E0, T0, eta, nu_x, xi_x, nu_y, xi_y;
  bool check =  GlobParas.get_parameter("E0",E0) &&
      GlobParas.get_parameter("T0",T0) &&
      GlobParas.get_parameter("eta",eta) &&
      GlobParas.get_parameter("x_betatron_tune",nu_x) &&
      GlobParas.get_parameter("x_lin_chromaticity",xi_x) &&
      GlobParas.get_parameter("y_betatron_tune",nu_y) &&
      GlobParas.get_parameter("y_lin_chromaticity",xi_y);
  if(!check){
    throw std::runtime_error("Insufficient global parameters for Hamiltonian update");
  }
  Bunch B = Bunches[bunch_index];
  uint64_t num_particles = B.sim_parts.size();
  double delta_old, tau_old, x_trans_old, px_trans_old, y_trans_old, py_trans_old;
  double delta_new, tau_new, x_trans_new, px_trans_new, y_trans_new, py_trans_new;
  double x_trans_phase, y_trans_phase;
  for(uint64_t i=0; i< num_particles; ++i){
// Extracting current coordinates of particle
    delta_old = B.sim_parts[i].getDelta();
    tau_old = B.sim_parts[i].getTau();
    x_trans_old = B.sim_parts[i].getXTrans();
    px_trans_old = B.sim_parts[i].getPXTrans();
    y_trans_old = B.sim_parts[i].getYTrans();
    py_trans_old = B.sim_parts[i].getPYTrans();
// Calculating new coordinates as per Eq 11-14 in paper
    delta_new = delta_old+Voltage(tau_old, bunch_index)/E0;
    tau_new = tau_old-T0*eta*delta_new;
// x coord
    x_trans_phase = 2*pi*(nu_x+xi_x*delta_old);
    x_trans_new = x_trans_old*cos(x_trans_phase)+px_trans_old*sin(x_trans_phase);
    px_trans_new = -x_trans_old*sin(x_trans_phase)+px_trans_old*cos(x_trans_phase);
// y coord
    y_trans_phase = 2*pi*(nu_y+xi_y*delta_old);
    y_trans_new = y_trans_old*cos(y_trans_phase)+py_trans_old*sin(y_trans_phase);
    py_trans_new = -y_trans_old*sin(y_trans_phase)+py_trans_old*cos(y_trans_phase);
// updating particle coordinate
    Bunches[bunch_index].sim_parts[i].update(tau_new, delta_new, x_trans_new, px_trans_new, y_trans_new, py_trans_new);
  }
}
void TimeEvolution::FPUpdate(int bunch_index){
  std::normal_distribution<double> QuantumFluctuations{0.0,2.0};
  double T0;
  double alpha_x, alpha_y,alpha_tau;
  double D_x, D_y, D_tau;
  double roll_x, roll_y, roll_tau;
  bool check =  GlobParas.get_parameter("alpha_tau",alpha_tau) &&
      GlobParas.get_parameter("T0",T0) &&
      GlobParas.get_parameter("alpha_x",alpha_x) &&
      GlobParas.get_parameter("alpha_y",alpha_y) &&
      GlobParas.get_parameter("alpha_tau",alpha_tau) &&
      GlobParas.get_parameter("Dis_x",D_x) &&
      GlobParas.get_parameter("Dis_y",D_y) &&
      GlobParas.get_parameter("Dis_tau",D_tau);
  if(!check){
    throw std::runtime_error("Insufficient global parameters for FP update");
  }
  Bunch B = Bunches[bunch_index];
  uint64_t num_particles = B.sim_parts.size();
  double delta_old, px_trans_old, py_trans_old;
  double delta_new, px_trans_new, py_trans_new;
  for(uint64_t i=0; i< num_particles; ++i){
    // roll a gaussian of sigma 2 to simulate quantum fluctuations
    roll_x = QuantumFluctuations(generator);
    roll_y = QuantumFluctuations(generator);
    roll_tau = QuantumFluctuations(generator);
    // get old coordinates
    delta_old = B.sim_parts[i].getDelta();
    px_trans_old  = B.sim_parts[i].getPXTrans();
    py_trans_old  = B.sim_parts[i].getPYTrans();
    // calcualate new coordinates
    delta_new = delta_old-alpha_tau*T0*delta_old+sqrt(D_tau*T0)*roll_tau;
    px_trans_new = px_trans_old-alpha_x*T0*px_trans_old+sqrt(D_x*T0)*roll_x;
    py_trans_new = py_trans_old-alpha_y*T0*py_trans_old+sqrt(D_y*T0)*roll_y;
// Update coordinates
    Bunches[bunch_index].sim_parts[i].setDelta(delta_new); 
    Bunches[bunch_index].sim_parts[i].setPXTrans(px_trans_new); 
    Bunches[bunch_index].sim_parts[i].setPYTrans(py_trans_new); 
  }
}