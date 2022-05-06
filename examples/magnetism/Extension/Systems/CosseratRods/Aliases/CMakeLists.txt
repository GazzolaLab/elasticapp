# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY MagnetismExtensionCosseratRodAliases)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        MagnetizedCosseratRod.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PUBLIC
        MagnetizedCosseratRod.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CosseratRodComponents
        CosseratRodTraits
)
