#include "MPIHelper.h"

#include <cmath>
#include <vector>
#include <iostream>
#include <tuple>

#include "doctest.h"

namespace MPIHelper{

    void print_intervals(std::vector<std::tuple<int,int>> intervals){
        int count = 0;
        for(auto interval : intervals){
            std::cout << count << '\t' << std::get<0>(interval) << '\t' << std::get<1>(interval) << '\t' << std::endl;
            count++;
        }
    }
    
    std::vector<bool> bunch_locations(int n_bunches, int gap){
    // boolean vector containing information about where a bunch is located. True means a bunch exists, False means there is no bunch
    // Currently set up so that the gap occurs with the last couple of rf buckets. Can modify if needed
      std::vector<bool> locations;
        for(int i=0; i< n_bunches-gap; ++i){
          locations.push_back(true);
        }
        for(int i=n_bunches-gap; i< n_bunches; ++i){
          locations.push_back(false);
        }
      return locations;
    }


    std::vector<std::tuple<int,int>> generate_intervals(int world_size, int n_bunches){
    // stipulates which bunches get assigned to which processor assuming world_size processors and n_bunches bunches
    // consists of a vector of tuples. Each tuple is interpreted as a half-open interval on the positive integers.
    // The interval includes the lower bound and excludes the upper bound.
    // For example, std::tuple<int,int> range = {0,5} should be interpreted as the elements 0,1,2,3,4
    // we index the bunches with a 0 based index. (ie. 1st bunch is 0, 2nd bunch is 1 etc.)
      std::vector<std::tuple<int,int>> intervals;
      if(n_bunches <= 0){
        throw std::runtime_error("Number of bunches is currently non-positive");
      }
      if(world_size <=0){
          throw std::runtime_error("World size is currently non-positive");
      }
      if(world_size > n_bunches){
    // the largest possible value that world_size can have is equal to nbunches. This is because additional processors don't help speed up any calculations
        throw std::runtime_error("world size is number of bunches in partition function. Perhaps you flip-flopped the arguments?");
      }
    // the easiest case. just assign each bunch to it's own processor. Hence, each interval is [i,i+1) from i=0 to i=1319
      if(world_size == n_bunches){
          for(int processor=0; processor< world_size; ++processor){
            std::tuple<int,int> interval = std::make_tuple(processor, processor+1);
            intervals.push_back(interval);
          }
        return intervals;        
      }
    // Number of bunches assigned per processor, rounded up. Gaurenteed to be at least 1 due to above conditional
      int bunches_per_rounded_up = std::ceil( ((double) n_bunches) / ((double) world_size) );
    // The estimated number of bunches we need to simulate. Gaurenteed to be at least n_bunches
      int estimate =  bunches_per_rounded_up*world_size;
    // The number of bunches we are off by with out overestimate. Between 0 and world_size-1
      int margin = estimate-n_bunches;
        // number of processors that get assigned bunches_per_rounded_up bunches
          int n_processors_normal = world_size-margin;
          int reduced_bunches_per = bunches_per_rounded_up-1;
          int current_lower = 0-reduced_bunches_per;
          int current_upper = 0;
        // output intervals
          for(int processor=0; processor< margin; ++processor){
            current_lower += reduced_bunches_per;
            current_upper += reduced_bunches_per;
            std::tuple<int,int> interval = std::make_tuple(current_lower, current_upper);
            intervals.push_back(interval);
          }
          current_lower -= 1;
          for(int processor=0; processor < n_processors_normal; ++processor){
            current_lower += bunches_per_rounded_up;
            current_upper += bunches_per_rounded_up;
            std::tuple<int,int> interval = std::make_tuple(current_lower, current_upper);
            intervals.push_back(interval);
          }

      return intervals;
    }
}
    
TEST_CASE("Testing Generate Intervals") {
// all unique
  int bunches = 1320;
  int processors = 1320;    
  std::vector<std::tuple<int,int>> intervals_one  =   MPIHelper::generate_intervals(processors,bunches);
  CHECK(intervals_one.size() == processors);
  int lower_most = std::get<0>(intervals_one[0]);
  int upper_most = std::get<1>(intervals_one[intervals_one.size()-1]);
  CHECK(lower_most==0);
  CHECK(upper_most==bunches);
  for(int i=0; i< intervals_one.size()-1; ++i){
    int upper_bound_lower_index = std::get<1>(intervals_one[i]);
    int lower_bound_upper_index = std::get<0>(intervals_one[i+1]);
    CHECK(upper_bound_lower_index==lower_bound_upper_index);
  }
  intervals_one.clear();
// evenly dividing case
  bunches = 1320;
  processors = 11;    
  intervals_one  =   MPIHelper::generate_intervals(processors,bunches);
  CHECK(intervals_one.size() == processors);
  lower_most = std::get<0>(intervals_one[0]);
  upper_most = std::get<1>(intervals_one[intervals_one.size()-1]);
  CHECK(lower_most==0);
  CHECK(upper_most==bunches);
  for(int i=0; i< intervals_one.size()-1; ++i){
    int upper_bound_lower_index = std::get<1>(intervals_one[i]);
    int lower_bound_upper_index = std::get<0>(intervals_one[i+1]);
    CHECK(upper_bound_lower_index==lower_bound_upper_index);
  }
  intervals_one.clear();
// not evenly dividing case
  bunches = 1320;
  processors = 17;    
  intervals_one  =   MPIHelper::generate_intervals(processors,bunches);
  CHECK(intervals_one.size() == processors);
  lower_most = std::get<0>(intervals_one[0]);
  upper_most = std::get<1>(intervals_one[intervals_one.size()-1]);
  CHECK(lower_most==0);
  CHECK(upper_most==bunches);
  for(int i=0; i< intervals_one.size()-1; ++i){
    int upper_bound_lower_index = std::get<1>(intervals_one[i]);
    int lower_bound_upper_index = std::get<0>(intervals_one[i+1]);
    CHECK(upper_bound_lower_index==lower_bound_upper_index);
  }
  intervals_one.clear();
// only 1 processor
  bunches = 1320;
  processors = 1;    
  intervals_one  =   MPIHelper::generate_intervals(processors,bunches);
  CHECK(intervals_one.size() == processors);
  lower_most = std::get<0>(intervals_one[0]);
  upper_most = std::get<1>(intervals_one[intervals_one.size()-1]);
  CHECK(lower_most==0);
  CHECK(upper_most==bunches);
  for(int i=0; i< intervals_one.size()-1; ++i){
    int upper_bound_lower_index = std::get<1>(intervals_one[i]);
    int lower_bound_upper_index = std::get<0>(intervals_one[i+1]);
    CHECK(upper_bound_lower_index==lower_bound_upper_index);
  }
  intervals_one.clear();
}
