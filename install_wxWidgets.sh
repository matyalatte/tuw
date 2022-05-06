#!/bin/bash
#Builds and installs wxWidgets with cmake.

wx_version="3.1.5"

sudo apt -y install build-essential libgtk-3-dev

wget https://github.com/wxWidgets/wxWidgets/releases/download/v$wx_version/wxWidgets-$wx_version.tar.bz2
tar -xvjof wxWidgets-$wx_version.tar.bz2

cd wxWidgets-$wx_version

mkdir release
cd release
../configure prefix=/usr/local/ --disable-unicode --enable-monolithic --disable-shared
make -j$(nproc)
sudo make install
