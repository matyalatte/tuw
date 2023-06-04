#!/bin/bash
# Builds an executable file with cmake.
# SimpleCommandRunner will be generated in ../${build_type}

# You can specify build type as an argument like "bash build_exe.sh Release"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
else
    build_type="Release"
fi
echo "Build type: ${build_type}"

# wxWidgets version is defined in ./Simple-Command-Runner/WX_VERSION.txt
wx_version="$(cat $(dirname "$0")/../WX_VERSION.txt)"
install_dir="$HOME/wxWidgets-${wx_version}/${build_type}"
echo "wxWidgets location: ${install_dir}"

# Build
export PATH="${install_dir}":$PATH
export WX_CONFIG="${install_dir}"/wx-config
options="-D CMAKE_BUILD_TYPE=${build_type} -D BUILD_SHARED_LIBS=OFF"
echo "CMake arguments: ${options}"

pushd $(dirname "$0")/..
    mkdir ${build_type}
    cd ${build_type}
    cmake  ${options} ../
    cmake --build .

    # Strip symbols to reduce the binary size
    if [[ "$OSTYPE" == "linux-gnu"* ]]; then
        strip --strip-all SimpleCommandRunner
    elif [[ "$OSTYPE" == "darwin"* ]]; then
        strip SimpleCommandRunner
    fi
popd
