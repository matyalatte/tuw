#!/bin/bash
# Checks required versions of GLIBC and GLIBCXX.
# bash ./check_glibc_compatibility.sh path/to/your/binary

glibc_deps=$(objdump -T $1 | grep GLIBC_ | sed 's/.*GLIBC_\([.0-9]*\).*/\1/g' | sort -Vu)
glibcxx_deps=$(objdump -T $1 | grep GLIBCXX_ | sed 's/.*GLIBCXX_\([.0-9]*\).*/\1/g' | sort -Vu)
IFS=''
if [ "${glibc_deps}" != "" ]; then
    echo Required GLIBC versions;
    echo $glibc_deps
fi
if [ "${glibcxx_deps}" != "" ]; then
    echo Required GLIBCXX versions;
    echo $glibcxx_deps
fi
if [ "${glibc_deps}" == "" ] && [ "${glibcxx_deps}" == "" ]; then
    echo No GLIBC dependencies.
fi
