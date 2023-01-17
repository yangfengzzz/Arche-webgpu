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

# Assimp ##################################
cd assimp
mkdir build_release
cd build_release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j5
cd ..

mkdir build_debug
cd build_debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j5
cd ../..

# Jolt  ##################################
cd jolt/Build
mkdir build_release
cd build_release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j5
cd ..

mkdir build_debug
cd build_debug
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j5
cd ../../..

# GoogleTest ###############################
cd googletest
mkdir build
cd build
cmake ..
make -j5
cd ../..

# jsoncpp  ##################################
cd jsoncpp
mkdir build_release
cd build_release
cmake -DCMAKE_BUILD_TYPE=Release ..
make -j5
cd ../..

set +x
