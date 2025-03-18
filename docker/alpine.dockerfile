# Building workflow for distributions using musl.
#
# 1. Use this docker file to build the executable.
#    docker build -t tuw_alpine -f docker/alpine.dockerfile ./
#
# 2. Run the built image.
#    docker run --name tuw_alpine tuw_alpine
#
# 3. Use "docker cp" to get the built executable.
#    docker cp tuw_alpine:/Tuw/build/Release-Test/Tuw ./
#
# Notes:
#   -You can use buildx for cross compiling
#    sudo apt install -y qemu-user-static binfmt-support
#    docker buildx build --platform linux/arm64 -t tuw_alpine_arm -f docker/alpine.dockerfile ./
#
#   -You can run test.sh with build-arg
#    docker build --build-arg TEST=true -t tuw_alpine -f docker/alpine.dockerfile ./

# Base image
FROM alpine:3.16.5

# Install packages
RUN apk update && \
    apk add --no-cache \
        alpine-sdk linux-headers bash gtk+3.0-dev \
        py3-pip python3 ttf-freefont \
        xorg-server xvfb

# Need ttf-freefont to avoid a GTK warning
# "Native Windows wider or taller than * pixels are not supported"

# Install meson
RUN pip3 install meson==1.3.1 ninja==1.11.1

# Clone the repo
COPY . /Tuw

# Run test.sh when true
ARG TEST=false

# Build
WORKDIR /Tuw/shell_scripts
RUN if [ "$TEST" = "true" ]; then \
        Xvfb :99 -screen 0 1024x768x24 & \
        export DISPLAY=:99 && \
        ./test.sh; \
    else \
        ./build.sh; \
    fi
