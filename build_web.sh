#!/bin/bash

# exit if any command returns an error
set -e

# configure
emcmake cmake -B build/web

# build
cd build/web
TIMEFORMAT="Total build time: %Rs"
time emmake make -j`nproc`
cd ../..

# we're done!
echo Done! You will find your game\'s .html inside the build/web directory.

echo "Do you wish to run it?"
select yn in "Yes" "No"; do
    case $yn in
        Yes ) emrun build/web/*.html; break;;
        No ) exit;;
    esac
done
