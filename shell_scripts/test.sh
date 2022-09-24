#!/bin/bash
#Builds an executable file with cmake.
#SimpleCommandRunner will be generated in ../build

if [ "$1" = "Debug" ];
    then build_type="Debug";
    else build_type="Release";
fi

wx_version="$(cat $(dirname "$0")/../WX_VERSION.txt)"
install_dir=$HOME/wxWidgets-${wx_version}/${build_type}
export PATH=${install_dir}/bin:$PATH
export WX_CONFIG=${install_dir}/wx-config

pushd $(dirname "$0")/..
    mkdir ${build_type}Test
    cd ${build_type}Test
    cmake -D CMAKE_BUILD_TYPE=${build_type}\
        -D BUILD_SHARED_LIBS=OFF\
        -D BUILD_EXE=OFF\
        -D BUILD_TESTS=ON\
        ../
    cmake --build .
    ctest --verbose --output-on-failure
popd