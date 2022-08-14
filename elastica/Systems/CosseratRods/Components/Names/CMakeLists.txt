# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodComponents)

set(LIBRARY_SOURCES
        CosseratRod.hpp
        CosseratRodWithoutDamping.hpp
        Types.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)
