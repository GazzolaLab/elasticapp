# Automagically build elastica along with its dependencies

# ##############################################################################
# Distributed under the MIT License. See LICENSE for details.
# ##############################################################################
cmake_minimum_required(VERSION 3.13.0)

# ##############################################################################
# Set policies
# ##############################################################################
if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif ()
if (POLICY CMP0110)
    # disable support for arbitrary characters (such as whitespace) in test names
    # but rather use underscores
    cmake_policy(SET CMP0110 OLD)
endif ()

# ##############################################################################
# Versioning
# ##############################################################################
file(READ ${CMAKE_CURRENT_LIST_DIR}/elastica/ModuleSettings/Version.hpp ELASTICA_VERSION_FILE)

string(REGEX MATCH "#define ELASTICA_MAJOR_VERSION ([0-9]*)" _ELASTICA_MAJOR_VERSION ${ELASTICA_VERSION_FILE})
set(ELASTICA_MAJOR_VERSION ${CMAKE_MATCH_1})

string(REGEX MATCH "#define ELASTICA_MINOR_VERSION ([0-9]*)" _ELASTICA_MINOR_VERSION ${ELASTICA_VERSION_FILE})
set(ELASTICA_MINOR_VERSION ${CMAKE_MATCH_1})

string(REGEX MATCH "#define ELASTICA_PATCH_VERSION ([0-9]*)" _ELASTICA_PATCH_VERSION ${ELASTICA_VERSION_FILE})
set(ELASTICA_PATCH_VERSION ${CMAKE_MATCH_1})

# ##############################################################################
# Project details
# ##############################################################################
set(ELASTICA_VERSION "${ELASTICA_MAJOR_VERSION}.${ELASTICA_MINOR_VERSION}.${ELASTICA_PATCH_VERSION}")
set(ELASTICA_DESCRIPTION "Package to efficiently simulate soft-filaments on devices from your laptop to supercomputing clusters.")

project(elastica
        LANGUAGES CXX
        VERSION ${ELASTICA_VERSION}
        DESCRIPTION ${ELASTICA_DESCRIPTION})

# ##############################################################################
# Setting the directory which contains CMake modules
# ##############################################################################
# Appends the cmake path inside the MAKE_MODULE_PATH variable which stores the
# list of directories where to lookk for additional CMake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# Create a new (or overwrite existing) info file at start of configuration
file(WRITE "${CMAKE_BINARY_DIR}/BuildInformation.txt" "")


# ##############################################################################
# Including cmake only modules
# ##############################################################################
include(Logging)
set_logs_on()
set_log_level("DEBUG") # INFO  DEBUG  HEAVYDEBUG

option(DOCS_ONLY "Skip all initialization not required for rendering documentation" OFF)
option(ELASTICA_BUILD_BENCHMARKS "Build microbenchmarks" ON)
option(ELASTICA_BUILD_EXAMPLES "Build examples" OFF)
option(ELASTICA_BUILD_DOCUMENTATION "Builds the documentation in addition to code" ON)
option(ELASTICA_BUILD_PYTHON_BINDINGS "Build the python bindings for elastica" ON)

# Define standard installation directories before since python targets utilize
include(GNUInstallDirs)
# Disable `make install` depending on `make all` since we want to control what
# we install more closely. With this setting, and targets marked as `OPTIONAL`,
# only targets that were built will be installed.
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY ON)

include(ElasticaGetGitHash)
include(ElasticaSetSiteName)

if (ELASTICA_BUILD_DOCUMENTATION AND DOCS_ONLY)
    include(SetupDoxygen)
    return()
endif (ELASTICA_BUILD_DOCUMENTATION AND DOCS_ONLY)

include(ProhibitInSourceBuild)
include(CheckCompilerVersion)
#
include(ElasticaAddInterfaceLibraryHeaders)
include(ElasticaTargetHeaders)
include(ElasticaTargetSources)
include(ElasticaAddLibraries)
include(ElasticaSetupFlagsTarget)
# need flags to be defined
include(EnableWarnings)
#
include(SetBuildType)
include(SetupCCache)
include(SetupCraySupport)
include(SetCxxStandard)
include(SetupCxxFlags)
include(SetupElasticaInlining)
include(SetupMacOsx)
include(SetOutputDirectory)
include(SetupPic)
# include(SetupBuildInfo) include(SetupCCache)

# after setting standard
include(CheckBrokenArray0)

# In order to use certain code analysis tools like clang-tidy and cppcheck the
# compile commands need to be accessible. CMake can write these to a
# "compile_commands.json" file.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Need to load python libs before boost.python. include(SetupPythonLibs)
# include(SetupBlas)  include(SetupBoost) include(SetupBrigand)
# include(SetupCatch) include(SetupGoogleBenchmark) include(SetupGsl)
# include(SetupHdf5) include(SetupAllocator) include(SetupLIBXSMM)
# include(SetupLapack) include(SetupNumPy) include(SetupOpenMP)
# include(SetupPapi) include(SetupSciPy) include(SetupLibsharp)

include(SetupBlaze)
include(SetupBlazeTensor)
include(SetupBrigand)
include(SetupProfiling)
include(SetupSMP)
include(SetupStl)
include(SetupYamlCpp)
include(SetupFilesystem)

if (ELASTICA_BUILD_DOCUMENTATION)
    include(SetupDoxygen)
endif (ELASTICA_BUILD_DOCUMENTATION)
include(CodeCoverageDetection)

include(ElasticaSetupTesting)
if (BUILD_TESTING)
    include(SetupCatch)
    include(ElasticaAddTestLibs)
    include(ElasticaAddCatchTests)
endif ()

if (ELASTICA_BUILD_PYTHON_BINDINGS)
    include(SetupPybind11)
endif ()
# Don't guard the following include with ELASTICA_BUILD_PYTHON_BINDINGS to provide a uniform interface to the rest of
# the build system.
include(ElasticaSetupPythonPackage)

add_subdirectory(elastica)

if (BUILD_TESTING)
    add_subdirectory(tests)
endif (BUILD_TESTING)
if (ELASTICA_BUILD_BENCHMARKS)
    if (NOT BUILD_TESTING) # already included once
        include(SetupCatch)
    endif ()
    add_subdirectory(benchmarks)
endif (ELASTICA_BUILD_BENCHMARKS)

if (ELASTICA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (ELASTICA_BUILD_EXAMPLES)

include(PrintUsefulCMakeInfo)
include(ElasticaInstall)