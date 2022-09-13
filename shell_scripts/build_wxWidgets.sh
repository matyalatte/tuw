#!/bin/bash
#Builds and installs wxWidgets with cmake.

#you need build-essential and libgtk-3-dev for linux
#sudo apt -y install build-essential libgtk-3-dev

wx_version="$(cat ../WX_VERSION.txt)"

pushd ~/wxWidgets-$wx_version
mkdir release
cd release

../configure --disable-shared\
 --disable-sys-libs\
 --without-libpng\
 --without-libjpeg\
 --without-libtiff\
 --without-expat\
 --without-liblzma\
 --without-opengl\
 --disable-glcanvasegl\
 --disable-mediactrl\
 --disable-webview\
 --disable-pnm\
 --disable-gif\
 --disable-pcx\
 --disable-iff\
 --disable-svg

make -j$(nproc)
popd