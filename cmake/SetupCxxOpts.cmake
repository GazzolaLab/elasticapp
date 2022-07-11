log_info("Finding CxxOpts")

# cxxopts seems to do a good job of exporting its target, hence I skip
# creating a FindCxxOpts.cmake file
find_package(cxxopts 3.0 REQUIRED)

file(APPEND "${CMAKE_BINARY_DIR}/BuildInformation.txt"
        "cxxopts Version:  ${cxxopts_VERSION}\n")

get_target_property(INTERFACE_DIRECTORIES cxxopts::cxxopts INTERFACE_INCLUDE_DIRECTORIES)
list(GET INTERFACE_DIRECTORIES 0 cxxopts_INCLUDE_DIRS)

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        cxxopts
)
add_interface_lib_headers(
        TARGET cxxopts::cxxopts
        HEADERS
        cxxopts.h
)
mark_as_advanced(cxxopts_DIR) # ugly, so hide from user

log_debug("cxxopts_INCLUDE_DIRS: ${cxxopts_INCLUDE_DIRS}")
log_debug("cxxopts_VERSION: ${cxxopts_VERSION}")


