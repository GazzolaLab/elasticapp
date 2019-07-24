# Setting the project versions from the git tag Similar to
# https://stackoverflow.com/a/4318642

# Distributed under the MIT License. See LICENSE for details.

find_package(Git)
if(EXISTS "${CMAKE_SOURCE_DIR}/.git" AND ${GIT_FOUND})

    # Getting the current git working branch
    log_debug("Getting the current git working branch")
    set(GIT_BRANCH "" CACHE STRING "Current git working branch" FORCE)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_BRANCH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Getting COMMIT_HASH of the latest git commit: git log -1 --format=%h
    log_debug("Getting COMMIT_HASH of the latest git commit")
    set(GIT_COMMIT_HASH
        ""
        CACHE STRING "COMMIT_HASH of the latest git commit" FORCE)
    execute_process(COMMAND ${GIT_EXECUTABLE} rev-parse --short HEAD
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_COMMIT_HASH
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    # Getting the latest git version tag
    log_debug("Getting the latest git version tag")
    # set( GIT_TAG "" CACHE STRING "Latest git version tag" FORCE )
    execute_process(COMMAND ${GIT_EXECUTABLE} describe
                    WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                    OUTPUT_VARIABLE GIT_TAG
                    OUTPUT_STRIP_TRAILING_WHITESPACE)

    if(NOT GIT_TAG)
        set(GIT_TAG "v0.0.0-0.0000")
    else()
        string(REGEX
               REPLACE "^v([0-9]+)\\..*"
                       "\\1"
                       GIT_VERSION_MAJOR
                       "${GIT_TAG}")
        string(REGEX
               REPLACE "^v[0-9]+\\.([0-9]+).*"
                       "\\1"
                       GIT_VERSION_MINOR
                       "${GIT_TAG}")
        string(REGEX
               REPLACE "^v[0-9]+\\.[0-9]+\\.([0-9]+).*"
                       "\\1"
                       GIT_VERSION_PATCH
                       "${GIT_TAG}")
        set(GIT_VERSION
            "${GIT_VERSION_MAJOR}.${GIT_VERSION_MINOR}.${GIT_VERSION_PATCH}")
        set(PROJECT_FULL_VERSION ${GIT_TAG})
        set(PROJECT_VERSION_MAJOR ${GIT_VERSION_MAJOR})
        set(PROJECT_VERSION_MINOR ${GIT_VERSION_MINOR})
        set(PROJECT_VERSION_PATCH ${GIT_VERSION_PATCH})
        set(PROJECT_VERSION ${GIT_VERSION})
    endif()

else()
    log_debug("Using the default version")
endif()
