#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iomanip>
#include <fstream>
#include <filesystem>
#include <sstream>
#include <complex.h>

#include "fftw3.h"

#include <mpi.h>

#include "tclap/CmdLine.h"

#include "Eigen/Dense"

#include "Constants.h"
#include "ValidateInput.h"
#include "FileParser.h"
#include "Bunch.h"
#include "Parameters.h"
#include "Wakefield.h"
#include "TimeEvolution.h"

// Comment out below to temporarily disable MPI. Use if you want to debug single processor logic
#define MPI_ENABLED

// Comment out "#define DOCTEST_CONFIG_DISABLE" line to enable unit tests
#define DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char** argv){
  std::filesystem::path new_folder = data_folder_path;
  std::filesystem::create_directories(new_folder);
  #ifdef MPI_ENABLED
  MPI_Init(NULL, NULL);
  #endif
  std::string BunchParameterName;
  std::string CavityParameterName;
  std::string LatticeParameterName;
  std::string TimeEvolutionParameterName;
  std::string WakefieldParameterName;
  std::string RunTag;
  bool verbose;
  try{
// setting up TCLAP to parse command line
    TCLAP::CmdLine cmd("SPACECPP", ' ', "1.0");
// Bunch parameters
    TCLAP::ValueArg<std::string> BunchFile("b","BunchFileName","Input .yaml file for Bunch Parameters",false,"../configs/BunchParameters.yaml","string");
    cmd.add( BunchFile );
// Cavity Parameters
    TCLAP::ValueArg<std::string> CavityFile("c","CavityFileName","Input .yaml file for Cavity Parameters",false,"../configs/CavityParameters.yaml","string");
    cmd.add( CavityFile );
// Lattice Parameters
    TCLAP::ValueArg<std::string> LatticeFile("l","LatticeFileName","Input .yaml file for Lattice Parameters",false,"../configs/LatticeParameters.yaml","string");
    cmd.add( LatticeFile );
// TimeEvolution Parameters
    TCLAP::ValueArg<std::string> TimeEvoFile("t","TimeEvoFileName","Input .yaml file for Time Evolution Parameters",false,"../configs/TimeEvolution.yaml","string");
    cmd.add( TimeEvoFile );
// Wakefield Parameters
    TCLAP::ValueArg<std::string> WakefieldFile("w","WakefieldFile","Input .yaml file for Wakefield Parameters",false,"../configs/WakefieldParameters.yaml","string");
    cmd.add( WakefieldFile );
// Unique identifier for this run
    TCLAP::ValueArg<std::string> RunIDTagCheck("r","RunTag","Identifier for this simulation run. Defaults to nothing",false,"","string");
    cmd.add( RunIDTagCheck );
// Verbose debugging flag
    TCLAP::SwitchArg VerboseDebugSwitch("d","debug","Print verbose debugging output", cmd, false);
    // reading in command line arguments
    cmd.parse(argc, argv);
    BunchParameterName = BunchFile.getValue();
    CavityParameterName= CavityFile.getValue();
    LatticeParameterName= LatticeFile.getValue();
    TimeEvolutionParameterName= TimeEvoFile.getValue();
    WakefieldParameterName= WakefieldFile.getValue();
    RunTag =  RunIDTagCheck.getValue();
    verbose = VerboseDebugSwitch.getValue();
  }
  catch(TCLAP::ArgException &e)
  {
    std::cerr << "error: " << e.error() << " for arg " << e.argId() << std::endl;
  }
// Don't perform unit testing if directive flag is off    
#ifndef DOCTEST_CONFIG_DISABLE
  doctest::Context ctx;
  ctx.applyCommandLine(argc, argv);
  ctx.setOption("no-breaks", true); 
  int res = ctx.run(); 
  return 0;
#endif
// get processor number
  int world_rank;
  MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
// Read input parameters
  Parameters GlobalParameters = Parameters();
    bool f = ReadLatticeParameters(LatticeParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << LatticeParameterName << std::endl;
      return -1;
  }
  else{
    if(verbose && world_rank==0)
    std::cout << "Sucessfully Parsed " << LatticeParameterName << std::endl;
  }
    f = ReadTimeEvolutionParameters(TimeEvolutionParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << TimeEvolutionParameterName << std::endl;
      return -1;
  }
  else{
    if(verbose&& world_rank==0)
    std::cout << "Sucessfully Parsed " << TimeEvolutionParameterName << std::endl;
  }
    
    std::vector<Wakefield> WakefieldData;
    f = ReadWakefieldParameters(WakefieldParameterName, WakefieldData, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << WakefieldParameterName << std::endl;
      return -1;
  }
  else{
    if(verbose&& world_rank==0)
    std::cout << "Sucessfully Parsed " << WakefieldParameterName << std::endl;
  }
    
  if(verbose&& world_rank==0){
    GlobalParameters.print();
  }
// Read in cavity configuration
    std::vector<std::unique_ptr<Cavity>> cavities;
    bool cavityRead = ReadCavityParameters(CavityParameterName, cavities);
    if(!cavityRead){
        std::cerr << "Couldn't read " << CavityParameterName << std::endl;    
        return -1;
    }
    else{
    if(verbose&& world_rank==0)
        std::cout << "Sucessfully Parsed " << CavityParameterName << std::endl;
    }
    if(verbose&& world_rank==0){
      for(const auto& cav : cavities){
        cav->print();
      }
    }

// Read in bunch configuration
    std::vector<Bunch> bunches;
    bool randomGenRead = ReadBunchParameters(BunchParameterName,bunches );
    if(!randomGenRead){
        std::cerr << "Couldn't read " << BunchParameterName << std::endl;            
      return -1;
    }
    else{
      if(verbose&& world_rank==0)
      std::cout << "Sucessfully Parsed " << BunchParameterName << std::endl;
    }
    TimeEvolution t  = TimeEvolution(cavities, bunches, GlobalParameters);
    if(verbose){
      t.run_simulation(1,1,0,1);    
    }
    else{
      t.run_simulation(1,1,0,0);    
    }
  #ifdef MPI_ENABLED
    MPI_Finalize();
  #endif

  return 0;
}
