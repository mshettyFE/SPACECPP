#!/bin/bash
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
