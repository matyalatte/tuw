#!/usr/bin/env bash
# Build an universal binary for macOS.
# Tuw will be generated in ../build/${build_type}

# You can specify build type as an argument like "./build_universal.sh Debug"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
    preset="--native-file presets/debug.ini"
else
    build_type="Release"
    preset="--native-file presets/release.ini -Dcpp_eh=none"
fi

echo "Build type: ${build_type}"

os=$(uname -s)
options=""
if [[ "$build_type" == "Release" ]]; then
    options="-Db_lto=true"
fi

pushd $(dirname "$0")/..
    meson setup build/${build_type} ${preset} -Dmacosx_universal=true -Dbuild_test=false ${options} || exit 1
    cd build/${build_type}
    meson compile -v || exit 1

    # Strip symbols to reduce the binary size
    if [ "${build_type}" = "Release" ]; then
        if [[ "$os" == "Linux" ]]; then
            strip --strip-all ./Tuw
        elif [[ "$os" == "Darwin" ]]; then
            strip ./Tuw
        fi
    fi
popd
