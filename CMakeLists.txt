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
# Project and host details
# ##############################################################################
project(elastica
        LANGUAGES CXX
        VERSION "${ELASTICA_MAJOR_VERSION}.${ELASTICA_MINOR_VERSION}.${ELASTICA_PATCH_VERSION}")

site_name(HOSTNAME)

# ##############################################################################
# Setting the directory which contains CMake modules
# ##############################################################################
# Appends the cmake path inside the MAKE_MODULE_PATH variable which stores the
# list of directories where to lookk for additional CMake modules.
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

# ##############################################################################
# Including cmake only modules
# ##############################################################################
include(Logging)
set_logs_on()
set_log_level("DEBUG") # INFO  DEBUG  HEAVYDEBUG

option(DOCS_ONLY "Skip all initialization not required for rendering documentation")
option(ELASTICA_SMP "Enable/Disable the shared-memory parallelization." ON)
option(ELASTICA_PROFILING_MODE "Enable/Disable intrusive code profiling." ON)
option(ELASTICA_BUILD_TESTS "Build test programs" ON)
option(ELASTICA_BUILD_BENCHMARKS "Build microbenchmarks" ON)
option(ELASTICA_BUILD_EXAMPLES "Build examples" OFF)
option(ELASTICA_BUILD_DOCUMENTATION "Builds the documentation in addition to code" ON)
option(ELASTICA_BUILD_PYTHON_BINDINGS "Build the python bindings for elastica" ON)

if (DOCS_ONLY)
    include(SetupDoxygen)
    return()
endif (DOCS_ONLY)
if (ELASTICA_BUILD_DOCUMENTATION)
    include(SetupDoxygen)
endif (ELASTICA_BUILD_DOCUMENTATION)

include(ProhibitInSourceBuild)
# include(AddSources) # Add function to add a set of source files.
# include(ElasticaUtils)
include(CheckCompilerVersion)
include(ElasticaSetupFlagsTarget)
include(SetGitVersion)
include(SetOutputDirectory)
include(SetBuildType)
include(SetCxxStandard)
include(SetupSafeInclude)
include(EnableWarnings)
include(SetupCxxFlags)
include(ElasticaAddInterfaceLibraryHeaders)
include(ElasticaTargetHeaders)
include(ElasticaTargetSources)
include(ElasticaAddLibraries)
include(CheckBrokenArray0)
# include(SetupBuildInfo) include(SetupCCache)

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

include(SetupSleef)
include(SetupBlaze)
include(SetupBlazeTensor)
include(SetupBrigand)
#include(SetupBoost)
if (ELASTICA_BUILD_PYTHON_BINDINGS)
    include(SetupPybind11)
    # Set system-wide for now, correct it later on
    set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif ()
include(ElasticaSetupPythonPackage)

if (ELASTICA_BUILD_TESTS OR ELASTICA_BUILD_BENCHMARKS)
    include(SetupCatch)
endif ()
include(SetupFilesystem)

if (ELASTICA_SMP)
    include(SetupTBB)
    set(ELASTICA_SMP 1)
    # target_compile_definitions(elastica INTERFACE ELASTICA_USE_TBB)
else ()
    set(ELASTICA_SMP 0)
endif ()

include(SetupYamlCpp)
# include(SetupVTK)

include(CodeCoverageDetection)

# Configure files
configure_file("${CMAKE_CURRENT_LIST_DIR}/elastica/Configuration/Parallel.hpp.in"
        "${CMAKE_CURRENT_LIST_DIR}/elastica/Configuration/Parallel.hpp")

if (ELASTICA_PROFILING_MODE)
    set(ELASTICA_PROFILING_MODE 1)
    # target_compile_definitions(elastica INTERFACE ELASTICA_USE_TBB)
else ()
    set(ELASTICA_PROFILING_MODE 0)
endif ()
configure_file("${CMAKE_CURRENT_LIST_DIR}/elastica/Configuration/Profiling.hpp.in"
        "${CMAKE_CURRENT_LIST_DIR}/elastica/Configuration/Profiling.hpp")

if (ELASTICA_BUILD_TESTS)
    enable_testing(true)
    include(ElasticaAddTestLibs)
    include(ElasticaAddCatchTests)
endif ()

# directly call Simulator/ABC instead of elastica/Simulator/ABC
include_directories(${CMAKE_SOURCE_DIR}/elastica)
if (ELASTICA_BUILD_TESTS)
    include_directories(${CMAKE_SOURCE_DIR}/tests/Unit)
endif ()
#include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(elastica)
if (ELASTICA_BUILD_TESTS)
    add_subdirectory(tests)
endif (ELASTICA_BUILD_TESTS)
if (ELASTICA_BUILD_BENCHMARKS)
    add_subdirectory(benchmarks)
endif (ELASTICA_BUILD_BENCHMARKS)
if (ELASTICA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif (ELASTICA_BUILD_EXAMPLES)

include(PrintUsefulCMakeInfo)
