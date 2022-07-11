# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodComponents)
add_subdirectory(detail)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Components.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        WithCircularCosseratRod.hpp
        WithSquareCosseratRod.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Components.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        WithCircularCosseratRod.hpp
        WithSquareCosseratRod.hpp
)
