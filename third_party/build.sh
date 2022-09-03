#!/bin/bash
set -x

# Dawn #####################################
cd dawn
cp scripts/standalone.gclient .gclient
gclient sync

mkdir cmake-release
cd cmake-release
cmake -DCMAKE_BUILD_TYPE=Release .. 
make -j5
cd ..

mkdir cmake-debug
cd cmake-debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j5
cd ../..

# Google Flags ##############################
cd gflags
mkdir builds
cd builds
cmake ..
make -j5
cd ../..

# Google Log ##############################
cd glog
mkdir build
cd build
cmake ..
make -j5
cd ../..

# GLFW ####################################
cd glfw
mkdir build
cd build
cmake ..
make -j5
cd ../..

# fmt #####################################
cd fmt
mkdir build
cd build
cmake ..
make -j5
cd ../..

# ktx #####################################
cd ktx
mkdir build
cd build
cmake ..
make -j5
cd ../..

set +x
