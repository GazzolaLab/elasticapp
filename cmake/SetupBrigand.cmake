# Distributed under the MIT License.
# See LICENSE.txt for details.

log_info("Finding Brigand")

find_path(BRIGAND_INCLUDE_DIRS "brigand/brigand.hpp")

log_debug("Brigand include: ${BRIGAND_INCLUDE_DIRS}")

add_library(Brigand INTERFACE IMPORTED)
target_include_directories(Brigand INTERFACE ${BRIGAND_INCLUDE_DIRS})

add_interface_lib_headers(
        TARGET Brigand
        HEADERS
        brigand/brigand.hpp
)

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        Brigand
)
