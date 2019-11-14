#!/usr/bin/env sh

set -ue

CMAKE_DIR=$(cd $(dirname $0) && pwd)

# Run CMake.
cmake -S "$CMAKE_DIR" \
  -B "$CMAKE_DIR/build_proj_xcode" \
  -G Xcode
