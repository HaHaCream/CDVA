#!/bin/sh
# build the CDVA Application
MYFLAGS="-mpopcnt -O3 -DNDEBUG -fopenmp -pipe"
# run configure with optimization flags
mkdir -p build
cd build
../configure --prefix=$HOME CFLAGS="${MYFLAGS}" CXXFLAGS="${MYFLAGS}"
# build the binaries
make
# install the binaries in ~/bin ("$HOME/bin")
make install


