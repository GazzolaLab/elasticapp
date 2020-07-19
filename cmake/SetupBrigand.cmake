# Distributed under the MIT License.
# See LICENSE.txt for details.

log_info("Finding Brigand")

find_package(Brigand REQUIRED)

log_debug("Brigand include: ${BRIGAND_INCLUDE_DIR}")

add_library(Brigand INTERFACE IMPORTED)
set_property(TARGET Brigand PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${BRIGAND_INCLUDE_DIR})

add_interface_lib_headers(
        TARGET Brigand
        HEADERS
        brigand/brigand.hpp
)

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        Brigand
)
