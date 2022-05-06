# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY MagnetismExtensionSystems)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        CosseratRods.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        CosseratRods.hpp
)

add_subdirectory(CosseratRods)
add_library(${LIBRARY}::CosseratRods ALIAS MagnetismExtensionCosseratRods)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Systems
        MagnetismExtensionCosseratRods
)
