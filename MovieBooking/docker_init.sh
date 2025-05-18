#!/usr/bin/env bash

ls -alh /project

./external/vcpkg/bootstrap-vcpkg.sh
./external/vcpkg/vcpkg install

doxygen --version
c++ --version

cmake --preset=linux-x64-debug && cmake --build --preset=linux-x64-debug
