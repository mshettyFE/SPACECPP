#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mpi.h>

#include "fftw3.h"

#include "FileParser.h"
#include "Constants.h"

int main(int argc, char** argv){
  if(argc < 2){
    std::cout << argv[0] << '\t' << "Input File Name" << std::endl;
    return -1;
  }
    
  std::unordered_map<std::string,std::shared_ptr<std::ofstream>> FileMapping;
  if(OpenOutputFiles(OutputFileNames,FileMapping)){
      for(std::string file: OutputFileNames){
        WriteToOutputFile(FileMapping,file,"test1 test2");
        WriteToOutputFile(FileMapping,file,"test3 test4");
    }
  }
    
  std::string fname = argv[1];
  std::unordered_map<std::string,double> ParameterMap;
  if(ReadContents(fname,ParameterMap)){
      PrintMap(ParameterMap);  
  }
    int N = 10;
    fftw_complex *in, *out;
    fftw_plan p;
    in = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    out = (fftw_complex*) fftw_malloc(sizeof(fftw_complex) * N);
    p = fftw_plan_dft_1d(N, in, out, FFTW_FORWARD, FFTW_ESTIMATE);
     fftw_destroy_plan(p);
    fftw_free(in); fftw_free(out);

//  MPI_Init(&argc, &argv);
//  MPI_Finalize();
  return 0;
}
