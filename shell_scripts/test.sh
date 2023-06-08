#!/bin/bash
# Builds tests with cmake.

# You can specify build type as an argument like "test.sh Release"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
else
    build_type="Release"
fi

# Build and test
pushd $(dirname "$0")/..
    preset="--preset ${build_type}-Unix-Test"
    cmake ${preset}
    cmake --build ${preset}
    ctest ${preset}
popd
