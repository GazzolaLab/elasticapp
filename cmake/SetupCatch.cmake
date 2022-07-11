# Finds and links with the header only test framework

# Obtained with thanks from https://github.com/sxs-collaboration/spectre

# Distributed under the MIT License. See LICENSE for details.

log_info("Finding Catch")

find_package(Catch 2.9.0 REQUIRED)

elastica_include_directories("${CATCH_INCLUDE_DIR}")
log_debug("CATCH_INCLUDE_DIR: ${CATCH_INCLUDE_DIR}")
log_debug("CATCH_VERSION: ${CATCH_VERSION}")

file(APPEND "${CMAKE_BINARY_DIR}/BuildInformation.txt"
        "Catch Version:  ${CATCH_VERSION}\n")
