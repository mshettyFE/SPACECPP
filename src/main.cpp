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
  std::string verbose_str;
// input yaml file names
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
  std::cout << std::setprecision (15);
// Don't perform unit testing if directive flag is off    
#ifndef DOCTEST_CONFIG_DISABLE
  // Redirect cerr to file
  std::ofstream err_file;
  std::string err_file_name = "TestingOutput.err";
  err_file.open(err_file_name);
  std::streambuf* stream_buffer_err_file = err_file.rdbuf();
  std::cerr.rdbuf(stream_buffer_err_file);
  doctest::Context ctx;
  ctx.applyCommandLine(argc, argv);
  ctx.setOption("no-breaks", true); 
  int res = ctx.run(); 
  std::cout << "All errors was written to " << err_file_name << std::endl;
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
// Read input parameters
  Parameters GlobalParameters = Parameters();
    bool f = ReadLatticeParameters(LatticeParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << LatticeParameterName << std::endl;
      return -1;
  }
  else{
    std::cout << "Sucessfully Parsed " << LatticeParameterName << std::endl;
  }
    f = ReadTimeEvolutionParameters(TimeEvolutionParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << TimeEvolutionParameterName << std::endl;
      return -1;
  }
  else{
    std::cout << "Sucessfully Parsed " << TimeEvolutionParameterName << std::endl;
  }
    f = ReadWakefieldParameters(WakefieldParameterName, GlobalParameters);
  if(!f){
      std::cerr << "Couldn't read " << WakefieldParameterName << std::endl;
      return -1;
  }
  else{
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
      std::cout << "Sucessfully Parsed " << BunchParameterName << std::endl;
    }
    bunches[0].write_data("TestBunch.csv");
    TimeEvolution t  = TimeEvolution(cavities, bunches, GlobalParameters);
    t.run_simulation(1,1,0,1);
  return 0;
}
