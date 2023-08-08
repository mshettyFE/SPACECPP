#!/bin/bash
# very simple script that runs cmake with mpicxx compiler, then runs make
mpi_loc=`which mpicxx`
cmake .. -DCMAKE_CXX_COMPILER=$mpi_loc
make