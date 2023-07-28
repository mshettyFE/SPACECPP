#ifndef SPACE_CONSTANTS
#define SPACE_CONSTANTS

#include <vector>
#include <string>
#include <cmath>

// Fundamental constants
    const double pi = acos(-1);
    // in meters per second
    const double c = 299792458;
    // Mass of electron in eV
    const double ElectronRestMass = 0.510998910;
    // Classical electron radius in m
    const double ClassicalElectronRadius = 2.8179409*1E-15;
    // Charge of electron in C
    const double ElectronCharge = 1.60217733E-19;
    // I don't know where the below came from???

// Output file names
    const std::vector<std::string> OutputFileNames = {
        "Absbunchmode0_1.dat",
        "Absbunchmode2_3.dat",
        "Fillpattern.dat",
        "Imbunchmode0_1.dat",
        "Landau_pap_deniCD.dat",
        "Landau_pap_densd_fin.dat",
        "Landau_pap_dens_fin.dat",
        "Landau_pap_dipd_fin.dat",
        "Landau_pap_dip_fin.dat",
        "Landau_pap_OUT.dat",
        "Landau_pap_ser4.dat",
        "Landau_pap_ser5.dat",
        "Landau_pap_sigd_bunc1160.dat",
        "Landau_pap_sigd_bunc1320.dat",
        "Landau_pap_sigd_bunc1.dat",
        "Landau_pap_sigd_bunc330.dat",
        "Landau_pap_sigd_bunc660.dat",
        "Landau_pap_sigd_bunc990.dat",
        "Landau_pap_sigs1.dat",
        "Landau_pap_sigs_bunc1160.dat",
        "Landau_pap_sigs_bunc1320.dat",
        "Landau_pap_sigs_bunc1.dat",
        "Landau_pap_sigs_bunc330.dat",
        "Landau_pap_sigs_bunc660.dat",
        "Landau_pap_sigs_bunc990.dat",
        "Landau_pap_sigs.dat",
        "Landau_pap_tot_pot_na.dat",
        "Landau_pap_tot_volt_na.dat",
        "Rebunchmode0_1.dat",
        "wake.dat"
    };

// Possible checks you can perform on a string to check if it can be converted to a double
  enum ValidityCheckingFlags {IS_VALID, MIN_EXCLUSIVE, MIN_INCLUSIVE, MAX_EXCLUSIVE, MAX_INCLUSIVE, MIN_EXCLUSIVE_MAX_EXCLUSIVE, MIN_INCLUSIVE_MAX_INCLUSIVE, MIN_EXCLUSIVE_MAX_INCLUSIVE, MIN_INCLUSIVE_MAX_EXCLUSIVE};
// Coordinates in phase space
  enum Coords {TAU, DELTA, X_TRANS, PX_TRANS};
// Allowed types that can be stored in Parameters object
  enum Type {INT, DOUBLE, STRING};
// Possible cavity types
  enum CavityType {ACTIVE, PASSIVE};

#endif