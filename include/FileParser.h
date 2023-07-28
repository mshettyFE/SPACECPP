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
#include "Bunch.h"
#include "Parameters.h"

// Validates yaml node to see if value assoiated with key is properly formatted, then write to Para
bool ValidateYAMLWrapper(Parameters& Para, YAML::Node CurNode, std::string key, Type t, ValidityCheckingFlags flag, double boundary1=0.0, double boundary2=0.0);

// Validates unordered map to see if value assoiated with key is properly formatted, then write to Para
bool ValidateUnorderedMapWrapper(Parameters& Para, std::unordered_map<std::string,std::string> map,std::string key, Type t, ValidityCheckingFlags flag, double boundary1=0.0, double boundary2=0.0);

// Validate that a probability distribution has the correct parameters
bool ValidateCoordinateWrapper(Coords coordinate, YAML::Node CurNode, std::unordered_map<Coords, std::unique_ptr<ProbDist>>& function_map, Parameters GlobalParas = Parameters());

bool ReadLatticeParameters(std::string fname, Parameters& Para);

bool ReadTimeEvolutionParameters(std::string fname, Parameters& Para);

bool ReadWakefieldParameters(std::string fname, Parameters& Para);

bool ReadCavityParameters(std::string fname, std::vector<std::unique_ptr<Cavity>>& cavities);

bool ReadBunchParameters(std::string fname, std::vector<Bunch>& bunches );

// Output file managment
bool OpenOutputFiles(std::vector<std::string> FileNames,std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMapping);
bool WriteToOutputFile(std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMap, std::string fname, std::string text);
#endif
