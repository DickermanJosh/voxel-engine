#!/bin/bash

# Remove the build directory if it exists
if [ -d "build" ]; then
    rm -r build
fi

mkdir build
cd build
cmake ..
make
./minecraft
