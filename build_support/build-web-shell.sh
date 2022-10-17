#!/bin/bash

set -e

DIR="$( cd "$( dirname "${BASH_SOURCE[0]}" )" >/dev/null 2>&1 && pwd )"
cd "$DIR/.."

BUSTUB_PRIVATE_VERSION="$(git -C "${DIR}/../.." describe --always --dirty)"
BUSTUB_PUBLIC_VERSION="$(git -C "${DIR}/../../bustub_public" describe --always --dirty)"
BUSTUB_BUILD_TIME="$(date +%Y%m%d)"

BUSTUB_SHELL_DIRECTORY="bustub"
BUSTUB_BPT_DIRECTORY="bpt-printer"

mkdir -p cmake-build-wasm
cd cmake-build-wasm
emcmake cmake .. -DCMAKE_BUILD_TYPE=MinSizeRel

rm -rf deploy
mkdir -p deploy
cp ../.gitignore deploy
cp ../LICENSE deploy
cp ../tools/wasm-shell/extra_files/vercel.json deploy

make -j$(nproc) wasm-shell
mkdir -p "deploy/${BUSTUB_SHELL_DIRECTORY}"
cp bin/bustub-wasm-shell.js "deploy/${BUSTUB_SHELL_DIRECTORY}"
cp bin/bustub-wasm-shell.wasm "deploy/${BUSTUB_SHELL_DIRECTORY}"
cp -a ../tools/wasm-shell/extra_files/index.html "deploy/${BUSTUB_SHELL_DIRECTORY}"
cp ../logo/bustub.svg "deploy/${BUSTUB_SHELL_DIRECTORY}"
sed -i '' "s|\${BUSTUB_PRIVATE_VERSION}|${BUSTUB_PRIVATE_VERSION}|" "deploy/${BUSTUB_SHELL_DIRECTORY}/index.html"
sed -i '' "s|\${BUSTUB_PUBLIC_VERSION}|${BUSTUB_PUBLIC_VERSION}|" "deploy/${BUSTUB_SHELL_DIRECTORY}/index.html"
sed -i '' "s|\${BUSTUB_BUILD_TIME}|${BUSTUB_BUILD_TIME}|" "deploy/${BUSTUB_SHELL_DIRECTORY}/index.html"

make -j$(nproc) wasm-bpt-printer
mkdir -p "deploy/${BUSTUB_BPT_DIRECTORY}"
cp bin/bustub-wasm-bpt-printer.js "deploy/${BUSTUB_BPT_DIRECTORY}"
cp bin/bustub-wasm-bpt-printer.wasm "deploy/${BUSTUB_BPT_DIRECTORY}"
cp -a ../tools/wasm-bpt-printer/extra_files/index.html "deploy/${BUSTUB_BPT_DIRECTORY}"
cp ../logo/bustub.svg "deploy/${BUSTUB_BPT_DIRECTORY}"
sed -i '' "s|\${BUSTUB_PRIVATE_VERSION}|${BUSTUB_PRIVATE_VERSION}|" "deploy/${BUSTUB_BPT_DIRECTORY}/index.html"
sed -i '' "s|\${BUSTUB_PUBLIC_VERSION}|${BUSTUB_PUBLIC_VERSION}|" "deploy/${BUSTUB_BPT_DIRECTORY}/index.html"
sed -i '' "s|\${BUSTUB_BUILD_TIME}|${BUSTUB_BUILD_TIME}|" "deploy/${BUSTUB_BPT_DIRECTORY}/index.html"

ls -alh "deploy/${BUSTUB_SHELL_DIRECTORY}"
ls -alh "deploy/${BUSTUB_BPT_DIRECTORY}"
