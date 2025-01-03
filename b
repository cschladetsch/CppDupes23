#!/bin/bash
set -e  # Exit on error

# Create and enter build directory
mkdir -p build
cd build

# Configure and build
cmake ..
cmake --build .

cd ..
