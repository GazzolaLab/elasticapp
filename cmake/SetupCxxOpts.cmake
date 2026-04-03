log_info("Finding CxxOpts")
find_package(cxxopts CONFIG REQUIRED)

log_debug("cxxopts_VERSION: ${cxxopts_VERSION}")

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