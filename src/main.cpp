#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mpi.h>
#include <sstream>

#include <complex.h>
#include "fftw3.h"

#include "FileParser.h"
#include "Constants.h"

// Comment out below to enable unit tests
//#define DOCTEST_CONFIG_DISABLE

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char** argv){
  std::string ParameterName;
  std::string verbose_str;
  std::string CavityParameterName;
  bool verbose;
  if(argc < 4){
    std::cout << argv[0] << '\t' << "Input_parameter_yaml Cavity_parameter_yaml Verbose(0 or 1)" << std::endl;
    return -1;
  }
  else{  
    ParameterName = argv[1];
    CavityParameterName = argv[2];
    std::stringstream stream;
    verbose_str = argv[3];
    stream << verbose_str;
    stream >> verbose;
  }
// Don't perform unit testing if directive flag is off    
#ifndef DOCTEST_CONFIG_DISABLE
  doctest::Context ctx;
  ctx.applyCommandLine(argc, argv);
  ctx.setOption("no-breaks", true); 
  int res = ctx.run(); 
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
  std::unordered_map<std::string,std::string> ParameterMap;
  bool f =ReadInputParameters(ParameterName, ParameterMap);
  if(f && verbose){
      PrintInputMap(ParameterMap);
  }
  else{
    return -1;
  }
// Read in cavity configuration
    std::vector<std::unique_ptr<Cavity>> cavities;
    bool cavityRead = ReadCavityParameters(CavityParameterName,ParameterMap, cavities);
    if(cavityRead && verbose){
      for(const auto& cav : cavities){
        cav->print();
      }
    }
    else{
      return -1;
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
