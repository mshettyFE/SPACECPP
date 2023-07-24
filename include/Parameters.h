#ifndef PARAMETERS
#define PARAMETERS

#include "Constants.h"
#include "ValidateInput.h"

#include <unordered_map>
#include <string>
#include <tuple>

class Parameters{
  private:
    // map containing the variable name as the keys
    // Each value is <the actual variable value, the type of the variable being stored>
    std::unordered_map<std::string,std::tuple<std::string,Type>> parameters;
  public:
    bool add_parameter(std::string name,std::string candidate_value, Type t);
    bool remove_parameter(std::string name);
    bool get_parameter(std::string name, int& var);
    bool get_parameter(std::string name, double& var);
    bool get_parameter(std::string name, std::string& var);
    void print();
};

#endif
