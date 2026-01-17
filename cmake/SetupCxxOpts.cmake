log_info("Finding CxxOpts")
find_package(cxxopts CONFIG REQUIRED)

log_debug("cxxopts_VERSION: ${cxxopts_VERSION}")

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