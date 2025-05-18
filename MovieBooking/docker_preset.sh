#!/usr/bin/env bash

set -e

echo "*** Building Docker Image ***"
./docker_build.sh

echo "*** Running Docker Container ***"
./docker_run.sh

