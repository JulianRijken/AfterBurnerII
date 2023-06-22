#!/bin/bash

# exit if any command returns an error
set -e

# configure
cmake -B build/linux

# build
TIMEFORMAT="Total build time: %Rs"
time cmake --build build/linux -- -j`nproc`

# we're done!
echo Done! You will find your game binary inside the build/linux directory.

echo "Do you wish to run it?"
select yn in "Yes" "No"; do
    case $yn in
        Yes ) find build/linux -maxdepth 1 -executable -type f -exec '{}' -v \; ; break;;
        No ) exit;;
    esac
done
