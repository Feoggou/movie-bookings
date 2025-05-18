#!/usr/bin/env bash

set -e
set -x

doxygen --version
which c++
c++ --version

# NOTE: vcpkg has newer version of cmake
cmake --version

pwd
ls -alh

mkdir external
cd external

git clone https://github.com/microsoft/vcpkg.git
cd vcpkg

./bootstrap-vcpkg.sh
./vcpkg install

cd ../../
pwd
ls -alh

cmake --preset=linux-x64-debug && cmake --build --preset=linux-x64-debug
