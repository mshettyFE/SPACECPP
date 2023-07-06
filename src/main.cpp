#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mpi.h>

#include <complex.h>
#include "fftw3.h"

#include "FileParser.h"
#include "Constants.h"

#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"

int main(int argc, char** argv){
  std::string fname;
  if(argc < 2){
    std::cout << argv[0] << '\t' << "Input File Name" << std::endl;
    return -1;
  }
  else{  
    fname = argv[1];
  }
  doctest::Context ctx;
  ctx.applyCommandLine(argc, argv);
  ctx.setOption("no-breaks", true); 
  int res = ctx.run();             
  if(ctx.shouldExit())              
    return res;                
    
  std::unordered_map<std::string,std::shared_ptr<std::ofstream>> FileMapping;
  if(OpenOutputFiles(OutputFileNames,FileMapping)){
      for(std::string file: OutputFileNames){
        WriteToOutputFile(FileMapping,file,"test1 test2");
        WriteToOutputFile(FileMapping,file,"test3 test4");
    }
  }
    
  std::unordered_map<std::string,double> ParameterMap;
  if(ReadContents(fname,ParameterMap)){
      PrintMap(ParameterMap);  
  }
//  MPI_Init(&argc, &argv);
//  MPI_Finalize();
    return res;
}
