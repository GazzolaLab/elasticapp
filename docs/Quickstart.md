\page quickstart_page Quickstart

[TOC]

This quickstart builds and runs the single-rod tutorial. For prerequisites and
installation options, see \ref installation "Installation".

Configure with examples enabled, then build the tutorial target:

\code{.sh}
cmake -B build -G Ninja \
  -D CMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -D BUILD_TESTING=OFF \
  -D ELASTICA_BUILD_BENCHMARKS=OFF \
  -D ELASTICA_BUILD_EXAMPLES=ON \
  -D ELASTICA_BUILD_PYTHON_BINDINGS=OFF
cmake --build build --target tutorial_single_rod --parallel
\endcode

Run it with:

\code{.sh}
./build/bin/tutorial_single_rod
\endcode

The program creates a straight Cosserat rod, reads its position through the
rod API, and prints the position array's shape. Continue with \ref
core_concepts_page "Core Concepts" or browse \ref examples_page "Case Examples".
