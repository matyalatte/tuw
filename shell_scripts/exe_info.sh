#!/bin/bash

echo Dynamic lib dependencies:
ext=""
if [[ "$OSTYPE" == "darwin"* ]]; then
    otool -L $1
elif [[ "$OSTYPE" == "linux-gnu"* ]]; then
    ldd $1
else
    ldd $1
    ext=.exe
fi
ls -l $1 | awk '{printf "Executable size: %s\n", $5}'
