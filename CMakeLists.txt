# Automagically build elastica along with its dependencies

# ##############################################################################
# Distributed under the MIT License. See LICENSE for details.
# ##############################################################################
cmake_minimum_required(VERSION 3.3.2)

# ##############################################################################
# Set policies
# ##############################################################################
if (POLICY CMP0074)
    cmake_policy(SET CMP0074 NEW)
endif ()

# ##############################################################################
# Project and host details
# ##############################################################################
project(Elastica LANGUAGES CXX C)

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
include(SetupBlaze)
include(SetupTBB)
include(SetupCatch)
include(SetupBrigand)
include(SetupFilesystem)
# include(SetupVTK) include(SetupYamlCpp)

enable_testing(true)
include(ElasticaAddTestLibs)
include(ElasticaAddCatchTests)
include(SetupCompilationFailureTests)

include_directories(${CMAKE_SOURCE_DIR}/elastica)
#include_directories(${CMAKE_SOURCE_DIR})

add_subdirectory(elastica)
add_subdirectory(tests)
add_subdirectory(examples)

include(PrintUsefulCMakeInfo)
