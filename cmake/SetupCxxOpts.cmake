log_info("Finding CxxOpts")

# cxxopts seems to do a good job of exporting its target, hence I skip
# creating a FindCxxOpts.cmake file
find_package(cxxopts 3.0 REQUIRED)

file(APPEND "${CMAKE_BINARY_DIR}/LibraryVersions.txt"
        "cxxopts Version:  ${cxxopts_VERSION}\n")

get_target_property(INTERFACE_DIRECTORIES cxxopts::cxxopts INTERFACE_INCLUDE_DIRECTORIES)
list(GET INTERFACE_DIRECTORIES 0 cxxopts_INCLUDE_DIRS)

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        cxxopts
)

log_debug("cxxopts_INCLUDE_DIRS: ${cxxopts_INCLUDE_DIRS}")
log_debug("cxxopts_VERSION: ${cxxopts_VERSION}")


