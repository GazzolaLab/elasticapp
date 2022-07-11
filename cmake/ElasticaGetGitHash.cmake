# Distributed under the MIT License.
# See LICENSE.txt for details.
# Obtained with thanks from https://github.com/sxs-collaboration/spectre

set(GIT_HASH "")
set(GIT_BRANCH_COMMAND "")
set(GIT_BRANCH "")
set(GIT_DESCRIPTION_COMMAND "")
set(GIT_DESCRIPTION "")

if (EXISTS ${CMAKE_SOURCE_DIR}/.git)
    find_package(Git)

    if (Git_FOUND)
        execute_process(
                COMMAND ${GIT_EXECUTABLE} rev-parse HEAD
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_HASH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(GIT_BRANCH_COMMAND "${GIT_EXECUTABLE} rev-parse --abbrev-ref HEAD")
        execute_process(
                COMMAND bash -c "${GIT_BRANCH_COMMAND}"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_BRANCH
                OUTPUT_STRIP_TRAILING_WHITESPACE
        )
        set(GIT_DESCRIPTION_COMMAND "${GIT_EXECUTABLE} describe \
--always --first-parent --match 'v[0-9]*' HEAD")
        execute_process(
                COMMAND bash -c "${GIT_DESCRIPTION_COMMAND}"
                WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
                OUTPUT_VARIABLE GIT_DESCRIPTION
                OUTPUT_STRIP_TRAILING_WHITESPACE
                ERROR_VARIABLE GIT_DESCRIPTION_ERROR
                ERROR_STRIP_TRAILING_WHITESPACE
        )
        if (GIT_DESCRIPTION)
            log_info("Git description: ${GIT_DESCRIPTION}")
        else ()
            log_info("Could not determine git description ("
                    "${GIT_DESCRIPTION_ERROR}). Using commit hash instead.")
            set(GIT_DESCRIPTION ${GIT_HASH})
        endif ()
    endif ()
else ()
    log_info(STATUS "Not running in a git repository. Some features are disabled.")
endif ()

file(APPEND
        "${CMAKE_BINARY_DIR}/BuildInformation.txt"
        "Git description: ${GIT_DESCRIPTION}\n"
        "Git branch: ${GIT_BRANCH}\n"
        "Git hash: ${GIT_HASH}\n"
        )
