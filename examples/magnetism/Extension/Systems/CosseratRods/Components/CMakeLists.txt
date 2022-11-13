# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY MagnetismExtensionCosseratRodComponents)
add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Names)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        helpers.hpp
        Magnetism.hpp
        Tags.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        helpers.hpp
        Magnetism.hpp
        Names.hpp
        Tags.hpp
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ErrorHandling
        Utilities
        CosseratRodComponents
)
