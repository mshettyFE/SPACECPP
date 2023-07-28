#ifndef VALIDATE_INPUT
#define VALIDATE_INPUT

#include <unordered_map>
#include <string>

#include "yaml-cpp/yaml.h"

#include "Constants.h"

// Convert string to C++ double. Accepts Fortran format and C++ format
bool StrToDouble(std::string parameter_value_str,double& out_value);
// Boundary checking functions
bool ValidityChecking( std::string str_val, ValidityCheckingFlags flag, double boundary1=0.0, double boundary2=0.0);

#endif