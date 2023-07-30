cmake_minimum_required(VERSION 3.19 FATAL_ERROR)
# Require C++14
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set( CMAKE_CXX_COMPILER "/nsls2/software/ap/mpich/3.3.2-gcc-9.3.0/bin/mpicxx" )
# Define project name
project(SPACECPP)
# MPI stuff
find_package(MPI REQUIRED COMPONENTS CXX)
include_directories(MPI_INCLUDE_PATH)
set(CMAKE_CXX_COMPILE_FLAGS ${CMAKE_CXX_COMPILE_FLAGS} ${MPI_COMPILE_FLAGS})
set(CMAKE_CXX_LINK_FLAGS ${CMAKE_CXX_LINK_FLAGS} ${MPI_LINK_FLAGS})

#FFTW
set (FFTW3_LIBRARIES fftw3)
set (FFTW3_LIBRARY_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fftw/lib)
set (FFTW3_INCLUDE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/fftw/include)

#YAML
add_library(yaml SHARED IMPORTED) # or STATIC instead of SHARED
set_target_properties(yaml PROPERTIES
  IMPORTED_LOCATION "${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/lib/libyaml-cpp.so"
  INTERFACE_INCLUDE_DIRECTORIES "${CMAKE_CURRENT_SOURCE_DIR}/yaml-cpp/include"
)

# My own header files
file(GLOB FPSource src/FileParser.cpp)
set(FileParseName ${CMAKE_PROJECT_NAME}FileParser)
add_library(${FileParseName} SHARED ${FPSource})
target_include_directories (${CMAKE_PROJECT_NAME}FileParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_link_libraries(${CMAKE_PROJECT_NAME}FileParser yaml)
FILE(GLOB MyCSources *.c)
add_executable(SPACE src/main.cpp
  src/Particle.cpp
  src/Bunch.cpp
  src/PassiveCavity.cpp
  src/ActiveCavity.cpp
  src/Gaussian.cpp
  src/Quartic.cpp
  src/ProbDist.cpp
  src/ValidateInput.cpp
  src/Parameters.cpp
)
target_link_libraries(SPACE ${FileParseName})
target_link_libraries(SPACE ${FFTW3_LIBRARIES})
