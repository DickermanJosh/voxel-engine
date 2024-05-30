#!/bin/bash

# Remove the build directory if it exists
if [ -d "build" ]; then
    rm -r build
fi

mkdir build
cd build
cmake ..
make

# Check if the executable was built successfully
if [ ! -f "./minecraft" ]; then
    echo "Build failed."
    exit 1
fi

# Run the executable with gdb
gdb ./minecraft -ex "run" -ex "bt" -ex "quit"
