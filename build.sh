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

if [[ "$1" == "-d" ]]; then
    echo "Launching in GDB..."
    gdb ./minecraft -ex "run" -ex "bt" -ex "quit" | tee log.txt
else
    echo "Launching in normal mode..."
    ./minecraft 2>&1 | tee log.txt
fi
