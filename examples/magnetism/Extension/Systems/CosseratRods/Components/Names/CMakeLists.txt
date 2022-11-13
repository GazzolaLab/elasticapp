# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY MagnetismExtensionCosseratRodComponents)

set(LIBRARY_SOURCES
        MagnetizedCosseratRod.hpp
        MagnetizedCosseratRodWithoutDamping.hpp
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
