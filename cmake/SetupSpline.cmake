# Distributed under the MIT License.
# See LICENSE.txt for details.

log_info("Finding Spline")

find_package(Spline)

log_debug("Spline include: ${SPLINE_INCLUDE_DIR}")

add_library(Spline INTERFACE IMPORTED)
set_property(TARGET Spline PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${SPLINE_INCLUDE_DIR})

add_interface_lib_headers(
        TARGET Spline
        HEADERS
        spline.h
)

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        Spline
)
