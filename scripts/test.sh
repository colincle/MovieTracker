#!/bin/bash

mkdir -p build && \
cd build && \
cmake .. -DBUILD_TESTS=ON && \
make MovieTrackerTests && \
./tests/MovieTrackerTests
