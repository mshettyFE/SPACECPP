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
    bool remove_parameter(std::string name);
    bool add_parameter(std::string name,std::string candidate_value, Type t);
    template <typename Output>
    bool add_parameter(std::string name,Output candidate_value, Type t);
    template <typename Output>
    bool get_parameter(std::string name, Output& var);
    void print();
  private:
    template <typename Output>
    bool match_type(std::tuple<std::string, Type> val_tup, Output& var);
};

#endif
