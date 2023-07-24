#include <unordered_map>
#include <string>
#include <iostream>
#include <algorithm>
#include <cctype>

#include "ValidateInput.h"
#include "Constants.h"
#include "doctest.h"

bool StrToDouble(std::string parameter_value_str,double &out_value){
    // Convert double in string form to an double (accepts Fortran and C++ formats)
    // Empty string check
    if(parameter_value_str.empty()){
      std::cerr << "No value detected" << std::endl;
      return false;
    }
    // convert string to lower
    std::transform(parameter_value_str.begin(), parameter_value_str.end(), parameter_value_str.begin(), ::tolower);
    // search for 'd' in string
    std::string d = "d";
    size_t start_pos_d = parameter_value_str.find(d);
    // search for 'e' in string
    std::string e = "e";
    size_t start_pos_e = parameter_value_str.find(e);
    // neither d or e not found, we loop through string. The only valid characters are digits, periods and negative signs
    int location = -1;
    if((start_pos_d!=std::string::npos) && (start_pos_e!=std::string::npos)){
      std::cerr << "both d and e where found in the same double string" << std::endl;
      return false;
    }
    else if((start_pos_d == std::string::npos) && (start_pos_e == std::string::npos) ){
        bool check_for_period = true;
        for(auto character : parameter_value_str){
            location++;
          // Allow periods, but only once
          if(check_for_period && character=='.'){
            check_for_period = false;
            continue;
          }
          // All other characters must be valid digits
          else if(isdigit(character)){
            continue;
          }
          else if(character=='-'){
            // negative sign is the first character, indicating negative number.
            if(location==0){
                continue;            
            }
            // For this case, a negative sign should not appear anywhere else
            else{
              std::cerr << "Negative sign can only appear at the beginning for numbers not in scientific notation" << std::endl;
              return false;
            }
          }
          else{
            std::cerr << "Invalid character at location " << location << " in value " << parameter_value_str << std::endl;
            return false;
          }
        }
        // try-catch for just in case the above misses something
        try{
          out_value = std::stod(parameter_value_str);
        }
        catch(...){
            std::cerr << "Couldn't convert value to double" << std::endl;
            return false;
        }
    }
    // We found only 'd' in the string
    else if((start_pos_d!=std::string::npos) && (start_pos_e==std::string::npos)){
      std::string replacement = "e";
      parameter_value_str.replace(start_pos_d, d.length(), replacement);
      bool check_for_period = true;
      bool check_for_E_flag = true;
      int E_loc = -1;
      int current_loc = -1;
      for(auto character : parameter_value_str){
        current_loc++;
      // Allow periods, but only once
        if(check_for_period && character=='.'){
          check_for_period = false;
          continue;
        }
      // check for E, but only once, and note location of E
        else if(check_for_E_flag && character=='e'){
          check_for_E_flag = false;
          E_loc = current_loc;
          continue;
        }
        else if(character=='-'){
          // If the negative sign appears first, or directly after E, then we good
          // If E_loc hasn't been set to 1 yet, then E_loc+1=0 or the first character
          if(current_loc==(E_loc+1)){
            continue;
          }
          else{
            std::cerr << "Negative sign in incorrect place " << std::endl;
            return false;        
          }
        }
      // All other characters must be valid digits
        else if(isdigit(character)){
          continue;
        }
        else{
            std::cerr << "Invalid character at location " << current_loc << " in value " << parameter_value_str << std::endl;
          return false;
        }
      }
    // try-catch for just in case the above misses something
      try{
        out_value = std::stod(parameter_value_str);
      }
      catch(...){
          std::cerr << "Couldn't convert value to double" << std::endl;
          return false;
      }
    }
    // We found only 'e' in the string
    else{
      bool check_for_period = true;
      bool check_for_E_flag = true;
      int E_loc = -1;
      int current_loc = -1;
      for(auto character : parameter_value_str){
        current_loc++;
      // Allow periods, but only once
        if(check_for_period && character=='.'){
          check_for_period = false;
          continue;
        }
      // check for E, but only once, and note location of E
        else if(check_for_E_flag && character=='e'){
          check_for_E_flag = false;
          E_loc = current_loc;
          continue;
        }
        else if(character=='-'){
          // If the negative sign appears first, or directly after E, then we good
          // If E_loc hasn't been set to 1 yet, then E_loc+1=0 or the first character
          if(current_loc==(E_loc+1)){
            continue;
          }
          else{
            std::cerr << "Negative sign in incorrect place " << std::endl;
            return false;        
          }
        }
      // All other characters must be valid digits
        else if(isdigit(character)){
          continue;
        }
        else{
            std::cerr << "Invalid character at location " << current_loc << " in value " << parameter_value_str << std::endl;
          return false;
        }
      }
    // try-catch for just in case the above misses something
      try{
        out_value = std::stod(parameter_value_str);
      }
      catch(...){
          std::cerr << "Couldn't convert value to double" << std::endl;
          return false;
      }
    }
    return true;    
}

TEST_CASE("Testing StrToDouble...") {
    double burner;
    CHECK(StrToDouble("",burner) == false);
    CHECK(StrToDouble("123.23234234",burner) == true);    
    CHECK(StrToDouble("not_a_double",burner) == false);    
    CHECK(StrToDouble("3d9",burner) == true);
    CHECK(StrToDouble("3d9saf",burner) == false);
    CHECK(StrToDouble("-1123.345346",burner) == true);
    CHECK(StrToDouble("2.4d9",burner) == true);
    CHECK(StrToDouble("-2.4d9",burner) == true);
    CHECK(StrToDouble("-2.4d-9",burner) == true);
    CHECK(StrToDouble("-2.4d--9",burner) == false);
    CHECK(StrToDouble("-2.4E9",burner) == true);
    CHECK(StrToDouble("-2.4e9",burner) == true);
}


bool ValidityChecking(std::string string_val, ValidityCheckingFlags flag, double boundary1, double boundary2){
/*
  Used to validate that a value conforms to a constraint. Suppose that our number in question is x. We have 9 possible cases:
      * IS_VALID: Only checks if an value of a key is a valid real number
      The rest do validity checking in addition to boundary checking
      * MIN_EXCLUSIVE: checks if value is strictly greater than boundary1
      * MIN_INCLUSIVE: checks if value is greater than or equal to boundary1
      * MAX_EXCLUSIVE: checks if value is strictly less than boundary1
      * MAX_INCLUSIVE: checks if value is less than or equal to boundary1
      * MIN_EXCLUSIVE_MAX_EXCLUSIVE: checks if value is strictly greater than boundary1 and strictly less than boundary2
      * MIN_INCLUSIVE_MAX_INCLUSIVE:checks if value is  greater than or equal to boundary1 and less than or equal to boundary2
      * MIN_EXCLUSIVE_MAX_INCLUSIVE: checks if value is strictly greater than boundary1 and less than or equal to boundary2
      *MIN_INCLUSIVE_MAX_EXCLUSIVE: checks if value is greater than or equal to boundary1 and strictly less than boundary2
*/
  bool output = true;
  double val;
  if(StrToDouble(string_val,val)){
    switch (flag){
        case IS_VALID:
            break;
        case MIN_EXCLUSIVE:
            if(val <= boundary1){
              std::cerr << string_val << " must be strictly greater than " << boundary1 << std::endl;
              output= false;
            }
            break;
        case MIN_INCLUSIVE:
            if(val < boundary1){
              std::cerr << string_val << " must be greater than or equal to " << boundary1 << std::endl;
              output= false;
            }
            break;
        case MAX_EXCLUSIVE:
            if(val >= boundary1){
              std::cerr << string_val << " must be strictly less than " << boundary1 << std::endl;
              output = false;
            }
            break;
        case MAX_INCLUSIVE:
            if(val > boundary1){
              std::cerr << string_val << " must be less than or equal to " << boundary1 << std::endl;
              output = false;
            }
            break;
        case MIN_EXCLUSIVE_MAX_EXCLUSIVE:
            if((val <= boundary1) || (val >= boundary2) ){
              std::cerr << string_val << " must be strictly greater than " << boundary1 << " and strictly less than " << boundary2 <<  std::endl;
              output = false;
            }
            break;
        case MIN_INCLUSIVE_MAX_INCLUSIVE:
            if((val < boundary1) || (val > boundary2) ){
              std::cerr << string_val << " must be greater than or equal to" << boundary1 << " and less than or equal to " << boundary2 <<  std::endl;
              output =  false;
            }
            break;
        case MIN_EXCLUSIVE_MAX_INCLUSIVE:
            if((val <= boundary1) || (val > boundary2) ){
              std::cerr << string_val << " must be strictly greater than " << boundary1 << " and less than or equal to " << boundary2 <<  std::endl;
              output = false;
            }
            break;
        case MIN_INCLUSIVE_MAX_EXCLUSIVE:
            if((val < boundary1) || (val >= boundary2) ){
              std::cerr << string_val << " must be greater than or equal to" << boundary1 << " and strictly less than " << boundary2 <<  std::endl;
              output = false;
            }
            break;
        default:
// we got an invalid flag. we return false
            output = false;
    }
  }
  else{
    output = false;
  }
  if(!output){
    std::cerr << "Couldn't read " << string_val <<  std::endl;
  }
  return output;
}

TEST_CASE("Testing ValidityCheck...") {
    double min_val, max_val;
    // Is valid check
    CHECK(ValidityChecking("not_valid",IS_VALID) == false);
    CHECK(ValidityChecking("1231.23423",IS_VALID) == true);
    // Min exclusive check
    min_val = 100;
    CHECK(ValidityChecking("234.5671", MIN_EXCLUSIVE,min_val) == true);
    CHECK(ValidityChecking("100", MIN_EXCLUSIVE,min_val) == false);
    CHECK(ValidityChecking("10.1234", MIN_EXCLUSIVE,min_val) == false);
    // Min inclusive check
    CHECK(ValidityChecking("234.5671", MIN_INCLUSIVE,min_val) == true);
    CHECK(ValidityChecking("100", MIN_INCLUSIVE,min_val) == true);
    CHECK(ValidityChecking("10.1234", MIN_INCLUSIVE,min_val) == false);
    // Max exclusive check
    max_val = 1000;
    CHECK(ValidityChecking("1234.5671", MAX_EXCLUSIVE,max_val) == false);
    CHECK(ValidityChecking("100", MAX_EXCLUSIVE,max_val) == true);
    CHECK(ValidityChecking("1000", MAX_EXCLUSIVE,max_val) == false);
    // Min inclusive check
    CHECK(ValidityChecking("1234.5671", MAX_INCLUSIVE,max_val) == false);
    CHECK(ValidityChecking("100", MAX_INCLUSIVE,max_val) == true);
    CHECK(ValidityChecking("1000", MAX_INCLUSIVE,max_val) == true);
    // min exclusive max exclusive
    min_val = 10;
    max_val = 1000;
    CHECK(ValidityChecking("503.6", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == true);
    CHECK(ValidityChecking("10", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    CHECK(ValidityChecking("9.9", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    CHECK(ValidityChecking("1000", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    CHECK(ValidityChecking("1000.1", MIN_EXCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    // min inclusive max inclusive
    CHECK(ValidityChecking("503.6", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    CHECK(ValidityChecking("10", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    CHECK(ValidityChecking("9.9", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    CHECK(ValidityChecking("1000", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    CHECK(ValidityChecking( "1000.1", MIN_INCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    // min exclusive max inclusive
    CHECK(ValidityChecking("503.6", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    CHECK(ValidityChecking("10", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    CHECK(ValidityChecking("9.9", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    CHECK(ValidityChecking("1000", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == true);
    CHECK(ValidityChecking("1000.1", MIN_EXCLUSIVE_MAX_INCLUSIVE,min_val, max_val) == false);
    // min inclusive max exclusive
    CHECK(ValidityChecking("503.6", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == true);
    CHECK(ValidityChecking("10", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == true);
    CHECK(ValidityChecking("9.9", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    CHECK(ValidityChecking("1000", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);
    CHECK(ValidityChecking("1000.1", MIN_INCLUSIVE_MAX_EXCLUSIVE,min_val, max_val) == false);    
}