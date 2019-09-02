#!/usr/bin/env sh

CMAKE_DIR=$(cd $(dirname $0) && pwd)

# Create a working directory.
cd $CMAKE_DIR
mkdir -p build

# Run CMake.
cd build
cmake -GXcode ..
