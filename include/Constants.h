#ifndef SPACE_CONSTANTS
#define SPACE_CONSTANTS

#include <vector>
#include <string>
#include <cmath>

// Fundamental constants
    const double pi = acos(-1);
    // in meters per second
    const double light_speed = 299792458;
    // Mass of electron in eV
    const double ElectronRestMass = 0.510998910*1e6;
    // Classical electron radius in m
    const double ClassicalElectronRadius = 2.8179409*1E-15;
    // Charge of electron in C
    const double ElectronCharge = 1.60217733E-19;
    //used to scale input time to voltages (1e-12 corresponds to units of picoseconds for tau)
    const double time_scaling_factor = 1e-12;

// Output file names
   const std::string data_folder_path = "./data";

// Possible checks you can perform on a string to check if it can be converted to a double
  enum ValidityCheckingFlags {IS_VALID, MIN_EXCLUSIVE, MIN_INCLUSIVE, MAX_EXCLUSIVE, MAX_INCLUSIVE, MIN_EXCLUSIVE_MAX_EXCLUSIVE, MIN_INCLUSIVE_MAX_INCLUSIVE, MIN_EXCLUSIVE_MAX_INCLUSIVE, MIN_INCLUSIVE_MAX_EXCLUSIVE};
// Coordinates in phase space
  enum Coords {TAU, DELTA, X_TRANS, PX_TRANS, Y_TRANS,PY_TRANS};
// Allowed types that can be stored in Parameters object
  enum Type {INT, DOUBLE, STRING};
// Possible cavity types
  enum CavityType {ACTIVE, PASSIVE};

#endif