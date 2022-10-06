#!/bin/sh

cmake -B build -G Ninja -DCMAKE_BUILD_TYPE=Release -DARGPARSE_BUILD_TESTS=ON
ninja -C build
