#ifndef WAKEFIELD_DATA
#define WAKEFIELD_DATA

#include <string>
#include <vector>
#include <tuple>

// class that models wakefield function data taking from an input file
// Does the following:
// calculates discrete FFT
//  Performs cubic spline to enable smooth interpolation of function
// can resample to a different distribution via cubic spline resampling

class Wakefield{
  private:
    std::vector<std::tuple<double,double>> discrete_data;
    std::string fname;
    double min_tau;
    double max_tau;
  public:
    Wakefield(std::string path);
    void print();
  private:
    void read_file(std::string path);
};

#endif