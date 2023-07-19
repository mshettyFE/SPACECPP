#ifndef DOCTEST_CONFIG_IMPLEMENT
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest.h"
#endif

#ifndef FileParser
#define FileParser

#include <string>
#include <vector>
#include <fstream>
#include <memory>
#include <unordered_map>
#include <tuple>

#include "yaml-cpp/yaml.h"

#include "Constants.h"
#include "Cavity.h"

// Convert Fortran double to C++ double (Should probably extend to accept c++ format as well)
bool StrToDouble(std::string parameter_value_str,double& out_value);
// Make sure that the value associated with the key can be converted to a double
bool TestDouble(std::unordered_map<std::string,std::string> map, std::string key);

// Boundary checking functions
bool ValidityChecking(std::unordered_map<std::string,std::string> map, std::string key, ValidityCheckingFlags flag, double boundary1=0.0, double boundary2=0.0);

// Output File writing and printing
bool OpenOutputFiles(std::vector<std::string> FileNames,std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMapping);
// Write some text to a particular filestream
bool WriteToOutputFile(std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMap, std::string fname, std::string text);
// Input File parsing
bool ReadInputParameters(std::string fname,std::unordered_map<std::string,std::string>& InputFileMap);
// You need to update this function when you change the input parameter file to make sure that a particular input parameter is present
bool ValidateInputs(std::unordered_map<std::string,std::string> InputFileMap);
// Cavity parameter parsing
bool ReadCavityParameters(std::string fname, const std::unordered_map<std::string,std::string> InputFileMap, std::vector<std::unique_ptr<Cavity>>& cavities);
// Bunch parameter parsing
bool ReadBunchParameters(std::string fname, std::unordered_map<Coords, std::tuple<double,double>>& coord_parameters );
// Print input values 
void PrintInputMap(const std::unordered_map<std::string,std::string> Map);
// Print random number generation parameters 
void PrintRandomGenMap(const std::unordered_map<Coords, std::tuple<double,double>> Map);
#endif
