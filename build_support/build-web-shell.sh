#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR/.."

BUSTUB_PRIVATE_VERSION="$(git -C "${DIR}/../.." describe --always --dirty)"
BUSTUB_PUBLIC_VERSION="$(git -C "${DIR}/../../bustub_public" describe --always --dirty)"

mkdir -p cmake-build-wasm
cd cmake-build-wasm
emcmake cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel
make -j wasm-shell
cp -a ../tools/wasm-shell/extra_files/* bin/
sed -i '' "s|\${BUSTUB_PRIVATE_VERSION}|${BUSTUB_PRIVATE_VERSION}|" bin/index.html
sed -i '' "s|\${BUSTUB_PUBLIC_VERSION}|${BUSTUB_PUBLIC_VERSION}|" bin/index.html

ls -alh bin/
