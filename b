#!/bin/bash

# Build the project
#rm -rf build

mkdir -p build && cd build
cmake .. -DCMAKE_BUILD_TYPE=Release
cmake --build .

# Run all tests
#ctest --output-on-failure
test/fs2-test

# Navigate back to root
echo "Build and tests complete."
cd ..

