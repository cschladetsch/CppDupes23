#!/bin/bash
set -e  # Exit on error

# Create and enter build directory
mkdir -p build
cd build

# Configure and build
cmake ..
cmake --build .

# Run tests (adjust path based on where tests are built)
./test/fs2-test

cd ..
