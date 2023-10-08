#!/bin/bash
# Generate html report for coverage (./Tuw/coverage-report/index.html)
# It only works with GCC.

# You can specify build type as an argument like "bash coverage.sh Release"
if [ "$1" = "Debug" ]; then
    build_type="Debug"
else
    build_type="Release"
fi

pushd $(dirname "$0")/..
    cd build/${build_type}
    lcov --capture --directory ./ --output-file ./coverage.info
    lcov -e ./coverage.info '**/Tuw/include/*' '**/Tuw/src/*' --output-file ./coverage_filtered.info
    genhtml ./coverage_filtered.info --output-directory ../../coverage-report
popd
