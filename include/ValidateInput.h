#ifndef VALIDATE_INPUT
#define VALIDATE_INPUT

#include <unordered_map>
#include <string>

#include "yaml-cpp/yaml.h"

#include "Constants.h"

// Convert Fortran double to C++ double (Should probably extend to accept c++ format as well)
bool StrToDouble(std::string parameter_value_str,double& out_value);
// Boundary checking functions
bool ValidityChecking( std::string str_val, ValidityCheckingFlags flag, double boundary1=0.0, double boundary2=0.0);

#endif