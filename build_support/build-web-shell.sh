#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR/.."

BUSTUB_PRIVATE_VERSION="$(git -C "${DIR}/../.." describe --always --dirty)"
BUSTUB_PUBLIC_VERSION="$(git -C "${DIR}/../../bustub_public" describe --always --dirty)"

mkdir -p cmake-build-wasm
cd cmake-build-wasm
emcmake cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel

make -j$(nproc) wasm-shell
mkdir -p wasm-shell
cp bin/bustub-wasm-shell.js wasm-shell
cp bin/bustub-wasm-shell.wasm wasm-shell
cp -a ../tools/wasm-shell/extra_files/* wasm-shell/
sed -i '' "s|\${BUSTUB_PRIVATE_VERSION}|${BUSTUB_PRIVATE_VERSION}|" wasm-shell/index.html
sed -i '' "s|\${BUSTUB_PUBLIC_VERSION}|${BUSTUB_PUBLIC_VERSION}|" wasm-shell/index.html

make -j$(nproc) wasm-bpt-printer
mkdir -p wasm-bpt-printer
cp bin/bustub-wasm-bpt-printer.js wasm-bpt-printer
cp bin/bustub-wasm-bpt-printer.wasm wasm-bpt-printer
cp -a ../tools/wasm-bpt-printer/extra_files/* wasm-bpt-printer/
sed -i '' "s|\${BUSTUB_PRIVATE_VERSION}|${BUSTUB_PRIVATE_VERSION}|" wasm-bpt-printer/index.html
sed -i '' "s|\${BUSTUB_PUBLIC_VERSION}|${BUSTUB_PUBLIC_VERSION}|" wasm-bpt-printer/index.html

ls -alh wasm-shell/
ls -alh wasm-bpt-printer/
