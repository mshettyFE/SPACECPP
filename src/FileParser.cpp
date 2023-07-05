#include<fstream>
#include<iostream>
#include<string>

void PrintContents(std::string fname){
  std::string line;
  std::ifstream file (fname);
  if (file.is_open()){
    while( getline(file,line)){
      std::cout << line << '\n';
    }
    file.close();
  }
  else{
    std::cout << "Couldn't open file" << '\n';
  }
}
