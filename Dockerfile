# Building workflow for ubuntu 20.04 or later.
#
# 1. Use this docker file to build the executable.
#    docker build -t simple-command-runner ./
# 2. Run the built image.
#    docker run simple-command-runner
# 3. Check the created container id.
#    docker ps -a
# 4. Use "docker cp" to get the built executable.
#    docker cp <CONTAINER ID>:/Simple-Command-Runner/Release/SimpleCommandRunner ./

# Base image
ARG ubuntu_version="20.04"
FROM ubuntu:${ubuntu_version}

# Install packages
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update \
    && apt-get install -y --no-install-recommends \
       wget ca-certificates build-essential \
       openssl libssl-dev libgtk-3-dev git \
    && apt-get clean \
    && rm -rf /var/lib/apt/lists/*

# Downlaod Cmake3.25
ARG cmake_ver_short="3.25"
ARG cmake_version="${cmake_ver_short}.3"
WORKDIR /root/temp_cmake
RUN wget https://cmake.org/files/v${cmake_ver_short}/cmake-${cmake_version}.tar.gz; \
    tar -xzvf cmake-${cmake_version}.tar.gz

# Build Cmake3.25
WORKDIR /root/temp_cmake/cmake-${cmake_version}
RUN ./bootstrap; \
    make -j$(nproc); \
    make install

# Remove unnecessary files
WORKDIR /root
RUN rm -rf temp_cmake

# Clone the repo
WORKDIR /
RUN git clone https://github.com/matyalatte/Simple-Command-Runner.git

# Build wxWidgets
WORKDIR /Simple-Command-Runner/shell_scripts
RUN bash download_wxWidgets.sh; \
    bash build_wxWidgets.sh

# Build SimpleCommandRunner
WORKDIR /Simple-Command-Runner/shell_scripts
RUN bash build_exe.sh
