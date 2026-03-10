# Finds and links with the header only test framework

# Obtained with thanks from https://github.com/sxs-collaboration/spectre

# Distributed under the MIT License. See LICENSE for details.

log_info("Finding Catch")

find_package(Catch2 CONFIG REQUIRED)

if (Catch2_FOUND)
    log_debug("Found Catch2: ${Catch2_VERSION}")
    file(APPEND "${CMAKE_BINARY_DIR}/BuildInformation.txt"
        "Catch Version:  ${CATCH_VERSION}\n")

    add_library(Catch INTERFACE IMPORTED)
    target_link_libraries(Catch INTERFACE Catch2::Catch2 Catch2::Catch2WithMain)
else ()
    message(FATAL_ERROR "Catch2 not found")
endif()

