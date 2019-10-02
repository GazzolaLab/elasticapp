#!/usr/bin/env sh

BUILD_FOLDER="build"

# Pass arguments including FLAGS, COMPILERS etc.
cmake -B"${BUILD_FOLDER}" -H. "$@"

#cmake --build . --target elastica -- -j4
cmake --build "${BUILD_FOLDER}" --target elastica -- -j"$(nproc)"
