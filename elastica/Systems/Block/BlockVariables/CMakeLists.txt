# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY BlockVariables)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Aliases.hpp
        BlockInitializer.hpp
        BlockVariables.hpp
        Protocols.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Aliases.hpp
        BlockInitializer.hpp
        BlockVariables.hpp
        Protocols.hpp
        Types.hpp
        TypeTraits.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        BlockCore
)
