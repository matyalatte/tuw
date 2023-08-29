#!/bin/bash
# Build an executable.
# SimpleCommandRunner will be generated in ../build/${build_type}

# You can specify build type as an argument like "bash build.sh Debug"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
    options="-Dbuildtype=debug -Dlibui:buildtype=debug"
else
    build_type="Release"
    options="-Dbuildtype=release -Dlibui:buildtype=release -Db_ndebug=true -Dcpp_rtti=false -Db_lto=true"
fi
echo "Build type: ${build_type}"

common_opt="-Ddefault_library=static -Dlibui:default_library=static
 -Dlibui:tests=false -Dlibui:examples=false"

pushd $(dirname "$0")/..
    meson setup build/${build_type} ${common_opt} ${options} || exit 1
    cd build/${build_type}
    meson compile -v || exit 1

    # Strip symbols to reduce the binary size
    if [ "${build_type}" = "Release" ]; then
        if [[ "$OSTYPE" == "linux-gnu"* ]]; then
            strip --strip-all ./SimpleCommandRunner
        elif [[ "$OSTYPE" == "darwin"* ]]; then
            strip ./SimpleCommandRunner
        fi
    fi
popd
