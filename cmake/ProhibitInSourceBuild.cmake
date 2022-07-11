# Ensures out of source build only, else throw an error and stop the compilation
# process

# Distributed under the MIT License. See LICENSE for details.

# Obtained with thanks from https://github.com/sxs-collaboration/spectre

string(REGEX
        REPLACE "^${CMAKE_SOURCE_DIR}/([^/]*)/?.*$"
        "\\1"
        SUBDIR_OF_SOURCE_DIR
        ${CMAKE_BINARY_DIR})
set(PROHIBITED_SUBDIRS
        "benchmarks"
        "cmake"
        "contrib"
        "docs"
        "elastica"
        "examples"
        "external"
        "tests"
        "tools")

if ("${CMAKE_BINARY_DIR}" STREQUAL "${CMAKE_SOURCE_DIR}"
        OR ${SUBDIR_OF_SOURCE_DIR} IN_LIST PROHIBITED_SUBDIRS)
    message(
            FATAL_ERROR
            "\n"
            "You attempted to build ${PROJECT_NAME} in the directory:\n"
            "  ${CMAKE_BINARY_DIR}\n"
            "In-source builds, however, are not allowed. "
            "Please create a directory and run cmake from there, passing the path "
            "to the source directory as the last argument; for example:\n"
            "  cd ${CMAKE_SOURCE_DIR}\n"
            "  cmake -B build [OPTIONS] -S ${CMAKE_SOURCE_DIR}\n"
            "  cmake --build build\n"
            "You also need to remove the CMakeCache.txt file and the "
            "CMakeFiles directory in the source directory, or you will trigger "
            "this error again, even when doing an out-of-source build. Run:\n"
            "  rm -r ${CMAKE_BINARY_DIR}/CMakeCache.txt "
            "${CMAKE_BINARY_DIR}/CMakeFiles\n")
endif ()
