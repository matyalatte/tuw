# Building workflow for ubuntu 20.04 or later.
#
# 1. Use this docker file to build the executable.
#    docker build -t simple-command-runner ./
# 2. Run the built image. 
#    docker run simple-command-runner
# 3. Use "docker cp" to get the built executable.
#    docker cp <CONTAINER ID>:/Simple-Command-Runner/build/SimpleCommandRunner /<SOMEPATH>/SimpleCommandRunner

# Base image
ARG ubuntu_version="20.04"
FROM ubuntu:${ubuntu_version}

ENV DEBIAN_FRONTEND=noninteractive

# Install packages
RUN apt-get update
RUN apt-get install -y --no-install-recommends\
    wget ca-certificates build-essential libgtk-3-dev git cmake

# Install wxWidgets
ARG wx_version="3.1.5"
RUN wget https://github.com/wxWidgets/wxWidgets/releases/download/v${wx_version}/wxWidgets-${wx_version}.tar.bz2;\
    tar -xvjof wxWidgets-${wx_version}.tar.bz2;\
    rm -f wxWidgets-${wx_version}.tar.bz2
RUN mkdir wxWidgets-${wx_version}/release;\
    cd wxWidgets-${wx_version}/release;\
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
RUN cd wxWidgets-${wx_version}/release;\
    make -j$(nproc);\
    make install

# Build Simple Command Runner
RUN git clone https://github.com/matyalatte/Simple-Command-Runner.git
RUN mkdir Simple-Command-Runner/build;\
    cd Simple-Command-Runner/build;\
    cmake -DCMAKE_BUILD_TYPE=Release -D BUILD_SHARED_LIBS=OFF ../
RUN cd Simple-Command-Runner/build;\
    cmake --build .
