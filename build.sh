#!/bin/bash

# Stop on errors.
set -e -x

# C++ build output here. Note: JS and WASM output will go to 'public/'.
# See CMakeLists.txt.
BUILD_PATH="build/"
TOOLCHAIN_FILE="toolchain-arm-none-eabi.cmake"

BUILD_TARGET=mipi_dsi_lcd

# Initialize build.
cmake \
    -B ${BUILD_PATH} \
    -DCMAKE_TOOLCHAIN_FILE=${TOOLCHAIN_FILE} \
    -DBUILD_TARGET=${BUILD_TARGET} \
    -G Ninja

# Format.
clang-format-12 shared/**/*.h shared/**/*.cc -i

# Build.
cd ${BUILD_PATH} && ninja