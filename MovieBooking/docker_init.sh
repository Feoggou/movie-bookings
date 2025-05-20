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
git checkout 2025.04.09

./bootstrap-vcpkg.sh
./vcpkg install

cd ../../
pwd
ls -alh

cmake --preset=linux-x64-debug && cmake --build --preset=linux-x64-debug
ctest --preset=linux-x64-debug

find . -name MovieBooking -type f

./out/build/linux-x64-debug/src/MovieBooking