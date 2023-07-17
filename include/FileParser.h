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

#include "yaml-cpp/yaml.h"

#include "Cavity.h"

bool StrToDouble(std::string parameter_value_str,double& out_value);

bool TestDouble(std::unordered_map<std::string,std::string> map, std::string key);

bool AboveMinExclusive(std::unordered_map<std::string,std::string> map, std::string key, double min_val);

bool AboveMinInclusive(std::unordered_map<std::string,std::string> map, std::string key, double min_val);

bool BelowMaxExclusive(std::unordered_map<std::string,std::string> map, std::string key, double max_val);

bool BelowMaxInclusive(std::unordered_map<std::string,std::string> map, std::string key, double max_val);

bool SandwichedBetweenExclusive(std::unordered_map<std::string,std::string> map, std::string key, double min_val,double max_val);

bool SandwichedBetweenInclusive(std::unordered_map<std::string,std::string> map, std::string key, double min_val,double max_val);

// Output File writing and printing
bool OpenOutputFiles(std::vector<std::string> FileNames,std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMapping);
bool WriteToOutputFile(std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMap, std::string fname, std::string text);

// Input File parsing and printing
bool ReadInputParameters(std::string fname,std::unordered_map<std::string,std::string>& InputFileMap);
bool ValidateInputs(std::unordered_map<std::string,std::string> InputFileMap);
// Cavity parameter parsing
bool ReadCavityParameters(std::string fname, const std::unordered_map<std::string,std::string> InputFileMap, std::vector<std::unique_ptr<Cavity>>& cavities);

void PrintInputMap(const std::unordered_map<std::string,std::string> Map);
#endif
