#!/bin/bash
set -x

# PhysX #################################### 
# cd physx/physx

# cd compiler/linux-debug
# make clean
# cd ..

# cd linux-release
# make clean
# cd ../../../../

# GoogleTest ###############################
cd googletest
rm -rf build
cd ..

# Google Log ##############################
cd glog
rm -rf build
cd ..

# Google Flags ############################
cd gflags
rm -rf build
cd ..

# GLFW ####################################
cd glfw
rm -rf build
cd ..

# fmt ####################################
cd fmt
rm -rf build
cd ..

# ktx ####################################
cd ktx
rm -rf build
cd ..


set +x
