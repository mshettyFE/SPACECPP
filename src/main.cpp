#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <complex.h>

#include "fftw3.h"

#include <mpi.h>

#include "tclap/CmdLine.h"

#include "Constants.h"
#include "ValidateInput.h"
#include "FileParser.h"
#include "Bunch.h"
#include "Parameters.h"
#include "TimeEvolution.h"

// Comment out "#define DOCTEST_CONFIG_DISABLE" line to enable unit tests
#define DOCTEST_CONFIG_DISABLE
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char** argv){
    // Start MPI
  MPI_Init(NULL, NULL);
  std::string BunchParameterName;
  std::string CavityParameterName;
  std::string LatticeParameterName;
  std::string TimeEvolutionParameterName;
  std::string WakefieldParameterName;
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
// Verbose debugging flag
    TCLAP::SwitchArg VerboseDebugSwitch("d","debug","Print verbose debugging output", cmd, false);
    // reading in command line arguments
    cmd.parse(argc, argv);
    BunchParameterName = BunchFile.getValue();
    CavityParameterName= CavityFile.getValue();
    LatticeParameterName= LatticeFile.getValue();
    TimeEvolutionParameterName= TimeEvoFile.getValue();
    WakefieldParameterName= WakefieldFile.getValue();
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
// Read input parameters
  Parameters GlobalParameters = Parameters();
    bool f = ReadLatticeParameters(LatticeParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << LatticeParameterName << std::endl;
      return -1;
  }
  else{
    if(verbose)
    std::cout << "Sucessfully Parsed " << LatticeParameterName << std::endl;
  }
    f = ReadTimeEvolutionParameters(TimeEvolutionParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << TimeEvolutionParameterName << std::endl;
      return -1;
  }
  else{
    if(verbose)
    std::cout << "Sucessfully Parsed " << TimeEvolutionParameterName << std::endl;
  }
    f = ReadWakefieldParameters(WakefieldParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << WakefieldParameterName << std::endl;
      return -1;
  }
  else{
    if(verbose)
    std::cout << "Sucessfully Parsed " << WakefieldParameterName << std::endl;
  }

  if(verbose){
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
    if(verbose)
        std::cout << "Sucessfully Parsed " << CavityParameterName << std::endl;
    }
    if(verbose){
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
      if(verbose)
      std::cout << "Sucessfully Parsed " << BunchParameterName << std::endl;
    }
    TimeEvolution t  = TimeEvolution(cavities, bunches, GlobalParameters);
    if(verbose){
      t.run_simulation(1,1,0,1);    
    }
    else{
      t.run_simulation(1,1,0,0);    
    }
    MPI_Finalize();
  return 0;
}
