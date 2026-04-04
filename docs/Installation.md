\page installation Installation

[TOC]

This page describes the standard local build for Elastica++ on macOS and Linux.
For Python package installation after the build, see the
\ref python_installation "Python Installation" page.
For a first end-to-end local run after installation, continue with the
\ref simulation_setup_page "Simulation Setup Guide".

## Requirements

Elastica++ is built with CMake and uses `vcpkg` manifest mode for C++
dependencies.

- A C++20 compiler
- CMake 3.17 or newer
- Ninja or Make
- Python 3
- `vcpkg`

## Configure

Bootstrap `vcpkg` if it is not already available in your environment.
We recommend installing dependencies through `vcpkg`, although you may install dependencies yourself:
- `blaze` and `blaze-tensor`: linear-algebra
- `tbb`: shared-memory-parallelism
- `brigand`: meta-programming
- `catch2`: unit-test
- `yaml-cpp`, `cxxopts`

For some example cases, extra dependency might be needed:
- `pybind11`
- `spline`
- `HighFive` with `HDF5-mpi`: I/O

Configure Elastica++ with the `vcpkg` toolchain:

\code{.sh}
cmake -B build -G Ninja \
  -D CMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -D BUILD_TESTING=ON \
  -D ELASTICA_BUILD_BENCHMARKS=OFF \
  -D ELASTICA_BUILD_EXAMPLES=OFF \
  -D ELASTICA_BUILD_PYTHON_BINDINGS=OFF
\endcode

## Build

Build the project with:

\code{.sh}
cmake --build build --parallel
\endcode

## Python Bindings

Python bindings are optional and can be enabled with
`-D ELASTICA_BUILD_PYTHON_BINDINGS=ON`.

After building, the editable Python package lives in `build/bin/python` and can
be installed with:

```bash
cd build/bin/python
python3 -m pip install --editable .
```

This installs the `elasticapp` package in editable mode.

## Build documentation

Generate the Doxygen documentation without configuring the main package build:

\code{.sh}
cmake -B build-docs -G Ninja \
  -D ELASTICA_BUILD_DOCUMENTATION=ON
cmake --build build-docs --target doc --parallel
\endcode

If you enabled documentation in the main build, you can also build the docs
from that existing build tree.

## Test

Run the unit test suite from the build directory:

\code{.sh}
cd build
ctest --output-on-failure
\endcode

## Next steps

- \ref simulation_setup_page "Simulation Setup Guide" for the first runnable
  examples and for building a personal simulation
- \ref examples_page "Case Examples" for condensed example selection
