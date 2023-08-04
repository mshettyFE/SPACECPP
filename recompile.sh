#!/bin/bash
mpi_loc=`which mpicxx`
cmake .. -DCMAKE_CXX_COMPILER=$mpi_loc
make