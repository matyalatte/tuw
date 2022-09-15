#!/bin/bash
#Builds an executable file with cmake.
#SimpleCommandRunner will be generated in ../build

pushd $(dirname "$0")/..
mkdir build
cd build
cmake -DCMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=OFF ../
cmake --build .
popd