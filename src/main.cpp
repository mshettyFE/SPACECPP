#include <iostream>
#include <string>

#include "FileParser.h"

int main(int argc, char** argv){
  if(argc < 2){
    return -1;
  }
  std::string fname = argv[1];
  PrintContents(fname);
  return 0;
}
