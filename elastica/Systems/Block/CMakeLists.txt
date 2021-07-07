# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Systems)

add_subdirectory(Block)
add_subdirectory(BlockVariables)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Block.hpp
        Protocols.hpp
        Serialize.hpp
        Types.hpp
        TypeTraits.hpp
)

