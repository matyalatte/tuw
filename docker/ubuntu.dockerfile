# Building workflow for distributions using glibc.
#
# 1. Use this docker file to build the executable.
#    docker build -t tuw_ubuntu -f docker/ubuntu.dockerfile ./
#
# 2. Run the built image.
#    docker run -name tuw_ubuntu tuw_ubuntu
#
# 3. Use "docker cp" to get the built executable.
#    docker cp tuw_ubuntu:/Tuw/build/Release/Tuw ./
#
# Notes:
#   -You can use buildx for cross compiling
#    sudo apt install -y qemu-user-static binfmt-support
#    docker buildx build --platform linux/arm64 -t tuw_ubuntu -f docker/ubuntu.dockerfile ./
#
#   -You can run tests on the container.
#    docker build -t tuw_ubuntu -f docker/ubuntu.dockerfile ./
#    docker run --rm --init -i tuw_ubuntu xvfb-run bash test.sh

# Base image
ARG OS=ubuntu:20.04
FROM ${OS}

# Install packages
ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y --no-install-recommends \
            ca-certificates build-essential \
            libgtk-3-dev git python3-pip xvfb && \
    apt-get clean && \
    rm -rf /var/lib/apt/lists/*

# Install meson
RUN pip3 install meson ninja

# Clone the repo
COPY . /Tuw

# Build
WORKDIR /Tuw/shell_scripts
RUN bash build.sh
