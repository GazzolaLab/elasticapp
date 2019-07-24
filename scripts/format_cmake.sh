#!/bin/bash

command -v cmake-format >/dev/null 2>&1 &&
    find . \( -name "CMakeLists.txt" -o -name "*.cmake" \) -type f -print0 |
    xargs -0 cmake-format -i "$@"
