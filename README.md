# Elastica++

[![](https://img.shields.io/badge/c%2B%2B-14-blue.svg)](https://en.wikipedia.org/wiki/CollisionConfi%2B%2B#Standardization)
[![](https://img.shields.io/badge/license-MIT-blue.svg)]()

| **Build Status** | **Memo** | **Compiler** | **OS** |
|---|---|---|---|
| ![Test][Git_Cmake_Action_Badge] | Build Test | `GCC` > 10.0, `Clang` > 10 | Ubuntu-latest, MacOS-latest |

*Efficiently simulate soft-filaments on devices from your laptop to supercomputing clusters.*

[Add project logo.]

- [About](#sec-1)
- [Setup](#sec-2)
    - [Installation](#sec-2-1)
    - [Build](#sec-2-2)
- [Usage](#sec-x)
- [Development](#sec-3)
    - [Pre-Requisites](#sec-3-1)
    - [Development Environment](#sec-3-2)
    - [Build](#sec-3-3)
    - [Deployment](#sec-3-4)
- [Community](#sec-4)
    - [Contribution](#sec-4-1)
    - [Branches](#sec-4-2)
    - [Guideline](#sec-4-3)

# About<a id="sec-1"></a>

`Elastica++` is a `C++` package to simulate the dynamics of soft-filaments that, at every cross-section, can undergo all
six possible modes of deformation, allowing the filament to bend, twist, stretch and shear. Such slender structures are
ubiquitous in natural and artificial systems across scales, from DNA strands to flagella and space tethers. They also
make up a variety of complex systems, from birds’ nests made of twigs to living creatures made of bones, tendons, fibers
and muscles.

`Elastica++` models these filaments using Cosserat rod theory which allows for modeling assemblies of such rods while
interacting with complex exogenous (surface contact, friction and hydrodynamics) and endogenous (muscular activity)
effects. `Elastica++` maps such assemblies efficiently to multicore processors using a novel Block architecure which
allows it to scale from laptops to computing clusters. More information about `Elastica++` is available at the
Elastica [project website](https://cosseratrods.org)

## Setup<a id="sec-2"></a>

This project uses [CMake]+[Make] to build and [vcpkg] to manage C++ libraries. Using [C++]-14 features, it successfully
builds with AppleClang, [gcc] and [clang].

### Cluster Setup

To set-up the package for cluster, please refer [here](support/README.md).

### Installation<a id="sec-2-1"></a>

#### Short

If you use [Docker]:

```bash
docker pull acgetchell/cdt-plusplus
docker run -it --name cdt cdt-plusplus
```

Binaries will be in `/CDT-plusplus/build/src`. Proceed to [Usage](#sec-x).

### Long

On macOS or Linux, you will first need to install some prerequisites using your favorite package manager (
e.g. [homebrew] or [apt]):

- build-essential (Linux only)
- automake
- autoconf
- autoconf-archive
- libtool (macOS) or libtool-bin (Linux)
- texinfo
- yasm
- make (macOS) or ninja-build (Linux)

Next, install [vcpkg]:

```bash
git clone https://github.com/Microsoft/vcpkg.git
cd vcpkg
./bootstrap-vcpkg.sh -useSystemBinaries
./vcpkg integrate install
```

[vcpkg] is then invoked by CMake in [vcpkg manifest] mode and install the project dependencies listed in [vcpkg.json]
into a local `vcpkg_installed` directory.

### Build<a id="sec-2-2"></a>

The online documentation is hosted on XXX. You can find the API reference here. Write about how to use this project.

- Commands to start the project.

## Usage<a id="sec-x"></a>

## Development<a id="sec-3"></a>

If you want other people to contribute to this project, this is the section, make sure you always add this.

### Project Layout

The project is similar to the [PitchFork Layout], as follows:

- .github - GitHub specific settings
- build - Ephemeral out-of-source build directory
- contrib - Contribution and code guidelines
- cmake - Cmake configurations
- docs - Documentation
- external - Includes submodules of external projects (none so far, all using [vcpkg])
- scripts - Build, test, and run scripts
- elastica - Header and Source files
- tests - Unit tests

Each folder within the source code has a brief README explaining the layout for that
subsystem.

### Pre-Requisites<a id="sec-3-1"></a>

List all the pre-requisites the system needs to develop this project.

- A tool
- B tool

### Development Environment<a id="sec-3-2"></a>

Write about setting up the working environment for your project.

- How to download the project&#x2026;
- How to install dependencies&#x2026;

### Build<a id="sec-3-3"></a>

Write the build Instruction here.

### Deployment<a id="sec-3-4"></a>

Write the deployment instruction here.

## Community<a id="sec-4"></a>

If it&rsquo;s open-source, talk about the community here, ask social media links and other links.

### Contribution<a id="sec-4-1"></a>

Your contributions are always welcome and appreciated. Following are the things you can do to contribute to this
project.

1. **Report a bug**
    - If you think you have encountered a bug, and I should know about it, feel free to report it [[][here]] and I will
      take care of it.
2. **Request a feature**
    - You can also request for a feature [[][here]], and if it will viable, it will be picked for development.
3. **Create a pull request**
    - It can&rsquo;t get better then this, your pull request (PR) will be appreciated by the community. You can get
      started by picking up any open issues from [[][here]] and make a pull request. The steps to create a PR are
      detailed [here](#PR).

### Branches<a id="sec-4-2"></a>

[//]: # (I use an agile continuous integration methodology, so the version is frequently updated and development is really fast.)

1. `Master` is the development branch.

2. `Production` is the production branch.

3. No further branches should be created in the main repository.

**Steps to create a pull request<a id="PR"></a>**

1. Make a PR to `master` branch.
2. Please comply with best practices and guidelines, found in the contribution guidelines [here](contrib/README.md).
3. It must pass all continuous integration checks and get positive reviews. 

After this, changes will be merged.

[CMake]: https://www.cmake.org

[Clang]: https://clang.llvm.org

[gcc]: https://gcc.gnu.org/

[Catch]: https://github.com/catchorg/Catch2/blob/master/docs/Readme.md

[CTest]: https://gitlab.kitware.com/cmake/community/wikis/doc/ctest/Testing-With-CTest

[Doxygen]: http://www.doxygen.org

[CMakeLists.txt]: https://github.com/acgetchell/CDT-plusplus/blob/master/CMakeLists.txt

[TBB]: https://www.threadingbuildingblocks.org

[Make]: https://www.gnu.org/software/make/

[C++]: https://isocpp.org/

[Homebrew]: https://brew.sh

[apt]: https://wiki.debian.org/Apt

[vcpkg]: https://github.com/Microsoft/vcpkg

[//]: # (project specific)

[.travis.yml]: https://github.com/acgetchell/CDT-plusplus/blob/master/.travis.yml

[Doxyfile]: https://github.com/acgetchell/CDT-plusplus/blob/master/Doxyfile

[Pitchfork Layout]: https://api.csswg.org/bikeshed/?force=1&url=https://raw.githubusercontent.com/vector-of-bool/pitchfork/develop/data/spec.bs#tld.docs

[Git_Cmake_Action_Badge]: https://github.com/skim0119/elasticapp/actions/workflows/cmake-build.yml/badge.svg
