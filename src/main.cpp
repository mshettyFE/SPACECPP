#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mpi.h>
#include <fstream>
#include <sstream>
#include <complex.h>

#include "fftw3.h"

#include "Constants.h"
#include "ValidateInput.h"
#include "FileParser.h"
#include "Bunch.h"
#include "Parameters.h"

// Comment out below to enable unit tests
#define DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char** argv){
  std::string verbose_str;
  std::string BunchParameterName;
  std::string CavityParameterName;
  std::string LatticeParameterName;
  std::string TimeEvolutionParameterName;
  std::string WakefieldParameterName;
  bool verbose;
  if(argc < 7){
    std::cout << argv[0] << '\t' << "BunchParameterYAML CavityParameterYAML LatticeParameterYAML TimeEvolutionYAML WakefieldParameterYAML Verbose(0 or 1)" << std::endl;
    return -1;
  }
  else{  
    BunchParameterName = argv[1];
    CavityParameterName = argv[2];
    LatticeParameterName = argv[3];
    TimeEvolutionParameterName = argv[4];
    WakefieldParameterName = argv[5];
    std::stringstream stream;
    verbose_str = argv[6];
    // convert verbose argument to boolean
    stream << verbose_str;
    stream >> verbose;
  }
// Don't perform unit testing if directive flag is off    
#ifndef DOCTEST_CONFIG_DISABLE
  // Redirect cerr to file
  std::ofstream err_file;
  std::string err_file_name = "TestingOutput.err";
  err_file.open(err_file_name);
  std::streambuf* stream_buffer_err_file = err_file.rdbuf();
  std::cerr.rdbuf(stream_buffer_err_file);
  // Redirect cout to file
  std::ofstream out_file;
  std::string out_file_name = "TestingOutput.out";
  out_file.open(out_file_name);
  // Reference to original stout buffer
  std::streambuf* stream_buffer_cout = std::cout.rdbuf();
  std::streambuf* stream_buffer_out_file = out_file.rdbuf();
  std::cout.rdbuf(stream_buffer_out_file);
  doctest::Context ctx;
  ctx.applyCommandLine(argc, argv);
  ctx.setOption("no-breaks", true); 
  int res = ctx.run(); 
  std::cout.rdbuf(stream_buffer_cout);
  std::cout << "All errors was written to " << err_file_name << " and all output was written to " << out_file_name << std::endl;
  err_file.close();
  return 0;
#endif
// MPI test
/*
  MPI_Init(NULL, NULL);
    // Get the number of processes
    int world_size;
    MPI_Comm_size(MPI_COMM_WORLD, &world_size);
    // Get the rank of the process
    int world_rank;
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
    // Get the name of the processor
    char processor_name[MPI_MAX_PROCESSOR_NAME];
    int name_len;
    MPI_Get_processor_name(processor_name, &name_len);
    // Finalize the MPI environment.
        printf("Hello world from processor %s, rank %d"
           " out of %d processors\n",
           processor_name, world_rank, world_size);
    MPI_Finalize();
*/
// Opening output files
  std::unordered_map<std::string,std::shared_ptr<std::ofstream>> FileMapping;
  bool OpenFileSucess = OpenOutputFiles(OutputFileNames,FileMapping);
// Read input parameters
  Parameters GlobalParameters = Parameters();
    bool f = ReadLatticeParameters(LatticeParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << LatticeParameterName << std::endl;
      return -1;
  }
    f = ReadTimeEvolutionParameters(TimeEvolutionParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << TimeEvolutionParameterName << std::endl;
      return -1;
  }
    f = ReadWakefieldParameters(WakefieldParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << WakefieldParameterName << std::endl;
      return -1;
  }
  if(verbose){
    GlobalParameters.print();
  }
// Read in cavity configuration
    std::vector<std::unique_ptr<Cavity>> cavities;
    bool cavityRead = ReadCavityParameters(CavityParameterName, cavities);
    if(cavityRead && verbose){
      for(const auto& cav : cavities){
        cav->print();
      }
    }
    else{
      return -1;
    }

// Read in bunch configuration
    std::vector<Bunch> Bunches;
    bool randomGenRead = ReadBunchParameters(BunchParameterName,Bunches );
    if(!randomGenRead){
      return -1;
    }
    if(verbose){
      for(auto b : Bunches){
//        b.print();
      }    
    }
    
/*
// Defining useful combinations of input parameters
// Angular frequency around the ring
  double Omega_rf = 2*pi*ParameterMap["frf"];
// synchronous period around the ring
  double BasePeriod = 1/ParameterMap["frf"];
// Number of continuous adjacent bunches. Cast to integer
  int ContiguousAdjacentBunches = (int) ParameterMap["nbunches"]-(int) ParameterMap["igap"];
// The charge of the beam
  double BeamCharge = BasePeriod*ParameterMap["Aver_curr"];
// gamma factor of reference particle (Synchronous particle energy divided by rest mass of electron)
  double gamma_naught = ParameterMap["E0"]/ElectronRestMass;
// Circumference of the ring
    double Circumference = BasePeriod*c*sqrt(gamma_naught*gamma_naught-1)/gamma_naught;
// 
  double eta = ParameterMap["alpha_C"]-1.0/(gamma_naught*gamma_naught);
// double relative loss per turn
  double relative_loss = ParameterMap["Uloss"]/ParameterMap["vrf"];
// Synchronous particle phase
  double PhiSynchronousNaught = pi-asin(relative_loss);  
  double SinPhiSyncNaught = sin(PhiSynchronousNaught);
  double CosPhiSyncNaught = cos(PhiSynchronousNaught);
// Optimal Synchronous phase (Quartic)
  double OrderSquared = ParameterMap["Uloss"]*ParameterMap["Uloss"];
  double PhiSynchronousOptimal = pi-asin((OrderSquared/(OrderSquared-1))*(relative_loss));
// omega_naught is the small oscillation frequency for the quadratic potential
  double omega_naught = sqrt(eta*c*ParameterMap["vrf"]*Omega_rf*CosPhiSyncNaught/Circumference/ParameterMap["E0"]);
  // bunch length in quadratic potential in seconds
  double sig_quadratic = eta/omega_naught*ParameterMap["sig_d"];
  // Delta ranges from 0 to MaxDelta. 30 seems pretty arbitrary
  double RangeDelta = 30.0*ParameterMap["sig_d"];
*/
  return 0;
}
