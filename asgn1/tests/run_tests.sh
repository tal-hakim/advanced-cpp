#!/bin/bash

# Stop on first error
set -e

# Get the script directory
SCRIPT_DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )"

# Create the build directory if it doesn't exist
mkdir -p $SCRIPT_DIR/build

# Change to the build directory
cd $SCRIPT_DIR/build

# Build the project
echo "Building the project..."
cmake ..
make

# Run the tests
echo -e "\nRunning tests..."
./tank_game_tests

# Print message if all tests passed
if [ $? -eq 0 ]; then
    echo -e "\nAll tests passed!"
fi

echo -e "\nDone." 