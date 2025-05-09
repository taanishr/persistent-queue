#!/bin/bash

set -e  # Exit immediately if a command exits with a non-zero status

mkdir -p build
cd build
make clean || true  # Ignore errors if clean hasn't been run before
cmake -DCMAKE_BUILD_TYPE=Debug -DCMAKE_C_COMPILER="$(which clang)" -DCMAKE_CXX_COMPILER="$(which clang++)" -DCMAKE_EXPORT_COMPILE_COMMANDS=ON ..
make -j$(nproc)  # Use all available CPU cores for faster compilation
