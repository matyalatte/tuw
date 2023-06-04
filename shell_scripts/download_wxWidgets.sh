#!/bin/bash
# Downloads wxWidgets.

# wxWidgets version is defined in ./Simple-Command-Runner/WX_VERSION.txt
wx_version="$(cat $(dirname "$0")/../WX_VERSION.txt)"

pushd ~/
wget https://github.com/wxWidgets/wxWidgets/releases/download/v"$wx_version"/wxWidgets-"$wx_version".tar.bz2
tar -xvjof wxWidgets-"$wx_version".tar.bz2
rm -f wxWidgets-"$wx_version".tar.bz2
popd
