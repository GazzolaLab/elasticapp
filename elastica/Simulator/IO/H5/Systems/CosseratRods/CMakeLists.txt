# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY H5CosseratRods)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        CosseratRod.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        H5Utilities
)
