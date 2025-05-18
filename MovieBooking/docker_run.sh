#!/usr/bin/env bash

docker run --rm -it -v "$(pwd -W)":/project movie-booking:latest

