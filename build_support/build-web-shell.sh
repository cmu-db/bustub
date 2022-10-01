#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR/.."

mkdir -p cmake-build-wasm
cd cmake-build-wasm
emcmake cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make -j wasm-shell
