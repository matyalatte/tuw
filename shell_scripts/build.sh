#!/bin/bash
# Build an executable.
# Tuw will be generated in ../build/${build_type}

# You can specify build type as an argument like "bash build.sh Debug"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
    preset="--native-file presets/debug.ini"
else
    build_type="Release"
    preset="--native-file presets/release.ini"
fi

echo "Build type: ${build_type}"

pushd $(dirname "$0")/..
    meson setup build/${build_type} ${preset} || exit 1
    cd build/${build_type}
    meson compile -v || exit 1

    # Strip symbols to reduce the binary size
    if [ "${build_type}" = "Release" ]; then
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            strip --strip-all ./Tuw
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            strip ./Tuw
        fi
    fi
popd
