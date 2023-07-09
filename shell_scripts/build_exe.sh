#!/bin/bash
# Builds an executable file with cmake.
# SimpleCommandRunner will be generated in ../build/${build_type}

# You can specify build type as an argument like "bash build_exe.sh Release"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
else
    build_type="Release"
fi
echo "Build type: ${build_type}"

pushd $(dirname "$0")/..
    cmake --preset ${build_type}-Unix || exit 1
    cmake --build --preset ${build_type}-Unix || exit 1

    # Strip symbols to reduce the binary size
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        strip --strip-all ./build/${build_type}/SimpleCommandRunner
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        strip ./build/${build_type}/SimpleCommandRunner
    fi
popd
