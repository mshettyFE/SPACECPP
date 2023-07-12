cmake_minimum_required(VERSION 3.19.3 FATAL_ERROR)
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

# My own header files
file(GLOB FPSource src/FileParser.cpp)
set(FileParseName ${CMAKE_PROJECT_NAME}FileParser)
add_library(${FileParseName} SHARED ${FPSource})
target_include_directories (${CMAKE_PROJECT_NAME}FileParser PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
add_executable(SPACE src/main.cpp)
target_link_libraries(SPACE ${FileParseName})
target_link_libraries(SPACE ${FFTW3_LIBRARIES})
