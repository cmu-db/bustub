#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR/.."

BUSTUB_PRIVATE_VERSION="$(git -C "${DIR}/../.." describe --always --dirty)"
BUSTUB_PUBLIC_VERSION="$(git -C "${DIR}/../../bustub_public" describe --always --dirty)"

mkdir -p cmake-build-wasm
cd cmake-build-wasm
emcmake cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel

rm -rf deploy
mkdir -p deploy
cp ../.gitignore deploy
cp ../LICENSE deploy
cp ../tools/wasm-shell/extra_files/vercel.json deploy

make -j$(nproc) wasm-shell
mkdir -p deploy/shell
cp bin/bustub-wasm-shell.js deploy/shell
cp bin/bustub-wasm-shell.wasm deploy/shell
cp -a ../tools/wasm-shell/extra_files/index.html deploy/shell
cp ../logo/bustub.svg deploy/shell
sed -i '' "s|\${BUSTUB_PRIVATE_VERSION}|${BUSTUB_PRIVATE_VERSION}|" deploy/shell/index.html
sed -i '' "s|\${BUSTUB_PUBLIC_VERSION}|${BUSTUB_PUBLIC_VERSION}|" deploy/shell/index.html

make -j$(nproc) wasm-bpt-printer
mkdir -p deploy/bpt-printer
cp bin/bustub-wasm-bpt-printer.js deploy/bpt-printer
cp bin/bustub-wasm-bpt-printer.wasm deploy/bpt-printer
cp -a ../tools/wasm-bpt-printer/extra_files/index.html deploy/bpt-printer
cp ../logo/bustub.svg deploy/bpt-printer
sed -i '' "s|\${BUSTUB_PRIVATE_VERSION}|${BUSTUB_PRIVATE_VERSION}|" deploy/bpt-printer/index.html
sed -i '' "s|\${BUSTUB_PUBLIC_VERSION}|${BUSTUB_PUBLIC_VERSION}|" deploy/bpt-printer/index.html

ls -alh deploy/shell
ls -alh deploy/bpt-printer
