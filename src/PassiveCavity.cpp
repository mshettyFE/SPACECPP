#include "PassiveCavity.h"
#include "Bunch.h"
#include <iostream>
#include <unordered_map>
#include <string>

PassiveCavity::PassiveCavity(std::string cav_name, double shunt, double CavityPhase, double CavityOrder){
    name = cav_name;
    shunt_impedance = shunt;
    Phase = CavityPhase;
    order = CavityOrder;
    
};

double PassiveCavity::Voltage(double tau,const  Bunch bunch, const std::unordered_map<std::string,std::string> ParameterMap){
  return 0.0;
}

void PassiveCavity::print(){
  std::cout << "Cavity: " << name << "\t" << "Type: Passive" << '\t' <<  "Shunt Impedance:" << shunt_impedance << '\t' << "Phase: " << Phase << '\t' << "Order: " <<  order << std::endl;
}