#!/bin/bash
# Generate html report for coverage (./Simple-Command-Runner/coverage-report/index.html)
# It only works with GCC.

if [ "$1" = "Debug" ];
    then build_type="Debug";
    else build_type="Release";
fi

pushd $(dirname "$0")/..
    mkdir ${build_type}Test
    cd ${build_type}Test
    lcov --capture --directory ./ --output-file ./coverage.info
    lcov -e ./coverage.info '**/Simple-Command-Runner/include/*' '**/Simple-Command-Runner/src/*' --output-file ./coverage_filtered.info
    genhtml ./coverage_filtered.info --output-directory ../coverage-report
popd