# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY MagnetismExtensionCosseratRods)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Aliases.hpp
        Components.hpp
        CosseratRods.hpp
        Initializers.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Aliases.hpp
        Components.hpp
        CosseratRods.hpp
        Initializers.hpp
        Types.hpp
)

add_subdirectory(Aliases)
add_subdirectory(Components)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CosseratRods
        MagnetismExtensionCosseratRodAliases
        MagnetismExtensionCosseratRodComponents
)

add_subdirectory(Python)
