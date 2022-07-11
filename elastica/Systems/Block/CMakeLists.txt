# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Block)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Block)
add_library(${LIBRARY}::Core ALIAS BlockCore)

add_subdirectory(BlockVariables)
add_library(${LIBRARY}::Variables ALIAS BlockVariables)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Block.hpp
        Protocols.hpp
        Serialize.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Block.hpp
        Protocols.hpp
        Serialize.hpp
        Types.hpp
        TypeTraits.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        BlockCore
        BlockVariables
)
