# Distributed under the MIT License. See LICENSE.txt for details.
# Obtained with thanks from https://github.com/sxs-collaboration/spectre

log_info("Finding Blaze")

find_package(Blaze 3.2 REQUIRED QUIET)

log_debug("BLAZE_INCLUDE_DIR: ${BLAZE_INCLUDE_DIR}")
log_debug("BLAZE_VERSION: ${BLAZE_VERSION}")

file(APPEND "${CMAKE_BINARY_DIR}/LibraryVersions.txt"
        "Blaze Version:  ${BLAZE_VERSION}\n")

#elastica_include_directories("${BLAZE_INCLUDE_DIR}")
add_library(Blaze INTERFACE IMPORTED)
set_property(TARGET Blaze PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${BLAZE_INCLUDE_DIR})

add_interface_lib_headers(
        TARGET Blaze
        HEADERS
        blaze/math/DynamicMatrix.h
        blaze/math/DynamicVector.h
        blaze/math/StaticVector.h
        blaze/system/Version.h
)

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        Blaze
)

