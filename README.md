<div align='center'>
<h1> Elastica++</h1>

[![](https://img.shields.io/badge/c%2B%2B-20-blue.svg)](https://isocpp.org/)
[![CMake Build](https://github.com/GazzolaLab/elasticapp/actions/workflows/cmake-build.yml/badge.svg)](https://github.com/GazzolaLab/elasticapp/actions/workflows/cmake-build.yml)
[![Example Run](https://github.com/GazzolaLab/elasticapp/actions/workflows/examples-smoke-run.yml/badge.svg)](https://github.com/GazzolaLab/elasticapp/actions/workflows/examples-smoke-run.yml)
[![Documentation Build](https://github.com/GazzolaLab/elasticapp/actions/workflows/documentation-build.yml/badge.svg)](https://github.com/GazzolaLab/elasticapp/actions/workflows/documentation-build.yml)
</div>

Efficiently simulate soft filaments on devices ranging from laptops to
supercomputing clusters.

## About

`Elastica++` is a `C++` package for simulating the dynamics of soft filaments
that can bend, twist, stretch, and shear. These slender structures appear
across natural and engineered systems, from DNA strands and flagella to cables,
fibers, and soft robotic assemblies.

The package models these systems using Cosserat rod theory and supports
assemblies of rods interacting with complex exogenous effects such as contact,
friction, and hydrodynamics, as well as endogenous effects such as muscular
activity. `Elastica++` maps these assemblies efficiently to multicore
processors using its Block architecture. More background is available on the
[Elastica project website](https://cosseratrods.org).

## Build

```sh
cmake -B build -G Ninja \
  -D CMAKE_TOOLCHAIN_FILE="$VCPKG_ROOT/scripts/buildsystems/vcpkg.cmake" \
  -D BUILD_TESTING=OFF \
  -D ELASTICA_BUILD_BENCHMARKS=OFF \
  -D ELASTICA_BUILD_EXAMPLES=OFF \
  -D ELASTICA_BUILD_PYTHON_BINDINGS=OFF
cmake --build build --parallel
```

## Example/Demo

All demonstration cases are in `examples` directory including tutorials, case studies, and advanced examples (on paper). To install, set the flag `ELASTICA_BUILD_EXAMPLES=ON`.

## Documentation

Start with the [installation guide](docs/Installation.md), then browse the
[case examples](docs/Examples.md). The generated API reference is built from
the headers and examples with Doxygen.

To build yourself,
```sh
cmake -B build-docs -G Ninja \
  -D ELASTICA_BUILD_DOCUMENTATION=ON
cmake --build build-docs --target doc --parallel
```

## Repository Layout for Devs

- `elastica/`: core library sources and headers
- `cmake/`: CMake modules and project build helpers
- `docs/`: project documentation pages
- `tests/`: unit tests
- `benchmarks/`: benchmark targets
- `support/`: cluster setup and helper scripts

For cluster-specific setup notes, see [support/README.md](support/README.md).

## Contributing

Contributions are welcome.

- Report bugs or request features through the repository's GitHub issues.
- Open pull requests against the `main` branch.
- Follow the contribution guidance in [CONTRIBUTING.md](CONTRIBUTING.md).
- Ensure the relevant CI checks pass before requesting review.
