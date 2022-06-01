#!/bin/bash
#Uninstalls wxWidgets and remove resource files.

wx_version="$(cat ../WX_VERSION.txt)"

pushd ~/wxWidgets-$wx_version/release
sudo make uninstall
cd ../..
rm -rf wxWidgets-$wx_version
popd