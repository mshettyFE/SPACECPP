#!/bin/bash
# create FFTW library
tar -xzf fftw-3.3.10.tar.gz
mkdir fftw
cd fftw
DST=$PWD
cd ../fftw-3.3.10
./configure --enable-mpi --prefix=${DST} --enable-shared
make
make install
cd ..
rm -rf fftw-3.3.10
# create yaml parser library
cd yaml-cpp
mkdir build
cd build
cmake -DYAML_BUILD_SHARED_LIBS=on ..
make
cd ..
mkdir lib
cp build/libyaml-cpp.so* lib
rm -rf build
# create main executable
cd ..
mkdir build
cd build
cmake ..
make