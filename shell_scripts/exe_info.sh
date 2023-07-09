#!/bin/bash

pushd $(dirname "$0")/..
    cd build/Release
    echo Dynamic lib dependencies:
    ext=""
    if [[ "$OSTYPE" == "darwin"* ]]; then
        otool -L SimpleCommandRunner
    elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
        ldd SimpleCommandRunner
    else
        ldd SimpleCommandRunner.exe
        ext=.exe
    fi
    ls -l SimpleCommandRunner${ext} | awk '{printf "Executable size: %s\n", $5}'
popd
