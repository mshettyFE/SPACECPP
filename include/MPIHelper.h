#ifndef MPI_HELPER
#define MPI_HELPER

#include <vector>
#include <tuple>

void print_intervals(std::vector<std::tuple<int,int>> intervals);

std::vector<std::tuple<int,int>> generate_intervals(int world_size, int n_bunches);

#endif