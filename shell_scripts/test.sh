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

os=$(uname -s)
options=""
if [[ "$os" == "SunOS" ]]; then
    # Solaris requires gld to use these flags.
    options="-Db_lto=false -Db_coverage=false"
elif [[ "$os" == "NetBSD" ]]; then
    # NetBSD seems to have some issues about lto.
    options="-Db_lto=false"
elif [[ "$build_type" == "Release" ]]; then
    options="-Db_lto=true"
fi

# Build and test
pushd $(dirname "$0")/..
    meson setup build/${build_type}-Test ${preset} ${options} || exit 1
    cd build/${build_type}-Test
    meson compile -v || exit 1
    meson test -v || exit 1
popd
