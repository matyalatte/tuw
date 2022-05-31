#!/bin/bash
#Builds and installs wxWidgets with cmake.

#you need build-essential and libgtk-3-dev for linux
#sudo apt -y install build-essential libgtk-3-dev

wx_version="$(cat ../WX_VERSION.txt)"

pushd ~/wxWidgets-$wx_version/release
sudo make install
popd