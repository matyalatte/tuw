#!/bin/bash
# Run tests.

# You can specify build type as an argument like "bash test.sh Debug"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
    preset="--native-file presets/debug.ini --native-file presets/test.ini"
else
    build_type="Release"
    preset="--native-file presets/release.ini --native-file presets/test.ini"
fi
echo "Build type: ${build_type}"

if [[ "$OSTYPE" == "darwin"* ]]; then
    # You can build universal binaries with the secound argument like "bash test.sh Release Universal"
    if [ "$2" = "Universal" ]; then
        echo "Universal build: On"
    else
        preset="${preset} -Dmacosx_build_universal=false"
        echo "Universal build: Off"
    fi
fi

# Build and test
pushd $(dirname "$0")/..
    meson setup build/${build_type}-Test ${preset} || exit 1
    cd build/${build_type}-Test
    meson compile -v || exit 1
    meson test -v || exit 1
popd
