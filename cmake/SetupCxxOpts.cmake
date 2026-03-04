log_info("Finding CxxOpts")
if (TARGET cxxopts::cxxopts)
    log_info("Found existing cxxopts::cxxopts target, reusing it.")
    return()
endif ()

# cxxopts seems to do a good job of exporting its target, hence I skip
# creating a FindCxxOpts.cmake file
find_package(cxxopts 3.0 REQUIRED)

log_debug("cxxopts_INCLUDE_DIR: ${cxxopts_INCLUDE_DIR}")
log_debug("cxxopts_VERSION: ${cxxopts_VERSION}")

add_library(cxxopts::cxxopts INTERFACE IMPORTED)
set_property(TARGET cxxopts::cxxopts PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${cxxopts_INCLUDE_DIR})

file(APPEND "${CMAKE_BINARY_DIR}/BuildInformation.txt"
        "cxxopts Version:  ${cxxopts_VERSION}\n")

add_interface_lib_headers(
        TARGET cxxopts::cxxopts
        HEADERS
        cxxopts.hpp
)
set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        cxxopts::cxxopts
)