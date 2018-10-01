#!/bin/bash

rm -rf build
mkdir build
pushd build
cmake ../ -DCMAKE_INSTALL_PREFIX=$(pwd)/install
popd
