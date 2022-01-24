#!/bin/bash
set -x

# PhysX #################################### 
# cd physx/physx
# ./generate_projects.sh linux

# cd compiler/linux-debug
# make -j5
# cd ..

# cd linux-release
# make -j5
# cd ../../../../

# OZZ-Animation ###########################
# cd ozz
# mkdir build_release
# cd build_release
# cmake -DCMAKE_BUILD_TYPE=Release ..
# make -j5
# cd ..

# mkdir build_debug
# cd build_debug
# cmake -DCMAKE_BUILD_TYPE=Debug ..
# make -j5
# cd ../..

# GoogleTest ###############################
cd googletest
mkdir build
cd build
cmake ..
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


set +x
