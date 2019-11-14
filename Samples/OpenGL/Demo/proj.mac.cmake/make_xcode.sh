#!/usr/bin/env sh

set -ue

CMAKE_DIR=$(cd $(dirname $0) && pwd)

# Run CMake.
cmake \
  -S "$CMAKE_DIR" \
  -B "$CMAKE_DIR/build_make_xcode" \
  -D CMAKE_BUILD_TYPE=Release
cd "$CMAKE_DIR/build_make_xcode/" && make
