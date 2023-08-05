#ifndef MPI_HELPER
#define MPI_HELPER

#include <vector>
#include <tuple>
#include <string>
#include <mpi.h>

#include "cereal/types/vector.hpp"
#include "cereal/types/string.hpp"
#include "cereal/archives/binary.hpp"

namespace MPIHelper{
// Adapted from the following StackOverflow post: https://stackoverflow.com/questions/59568209/mpi-send-send-structure-as-vector
// serializes/deserealizes data as a string (which is basically a character array) so that it can be passed via MPI
// These aren't particularly efficient, since you have to copy over the data to a stringstream. I don't know the alternative though
    
template<class T>
std::string SerializeData(const T &data){
  std::stringstream ss;
  {
    cereal::BinaryOutputArchive archive( ss );
    archive( data );
  }
  return ss.str();
}

template<class T>
void DeserializeData(std::string input_data, T &output_data){
  std::stringstream ss;
  ss << input_data;

  //Unpack the data
  {
    cereal::BinaryInputArchive archive(ss);
    archive(output_data);
  }
}    
    
void print_intervals(std::vector<std::tuple<int,int>> intervals);

std::vector<std::tuple<int,int>> generate_intervals(int world_size, int n_bunches);

std::vector<bool> bunch_locations(int n_bunches, int gap);

}
#endif