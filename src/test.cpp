#include <iostream>
#include <string>
#include <vector>
#include <memory>
#include <unordered_map>
#include <mpi.h>


#include "FileParser.h"

int main(int argc, char** argv){
  if(argc < 2){
    std::cout << argv[0] << '\t' << "Input File Name" << std::endl;
    return -1;
  }
    
  std::vector<std::string> outFiles{"out1","out2","out3"};  
  std::unordered_map<std::string,std::shared_ptr<std::ofstream>> FileMapping;
    
  if(OpenOutputFiles(outFiles,FileMapping)){
      WriteToOutputFile(FileMapping,"out1","test1 test2");
      WriteToOutputFile(FileMapping,"out1","test3 test4");
      WriteToOutputFile(FileMapping,"out2","test5 test6");
      WriteToOutputFile(FileMapping,"out2","test7 test8");
      WriteToOutputFile(FileMapping,"out3","test9 test10");
      WriteToOutputFile(FileMapping,"out3","test11 test12");
  }
    
  std::string fname = argv[1];
  std::unordered_map<std::string,double> ParameterMap;
  if(ReadContents(fname,ParameterMap)){
      PrintMap(ParameterMap);  
  }
//  MPI_Init(&argc, &argv);
//  MPI_Finalize();
  return 0;
}
