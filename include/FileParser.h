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

bool StrToDouble(std::string parameter_value_str,double& out_value);

// Output File writing and printing
bool OpenOutputFiles(std::vector<std::string> FileNames,std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMapping);
bool WriteToOutputFile(std::unordered_map<std::string,std::shared_ptr<std::ofstream>>& FileMap, std::string fname, std::string text);

// Input File parsing and printing
bool ReadContents(std::string fname,std::unordered_map<std::string,double>& FileMapping);
void PrintMap(const std::unordered_map<std::string,double> Map);
#endif
