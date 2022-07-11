# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY BlockCore)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Aliases.hpp
        AsVariables.hpp
        Block.hpp
        BlockFacade.hpp
        BlockIterator.hpp
        BlockSlice.hpp
        DisplayVariables.hpp
        Protocols.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Aliases.hpp
        AsVariables.hpp
        Block.hpp
        BlockFacade.hpp
        BlockIterator.hpp
        BlockSlice.hpp
        DisplayVariables.hpp
        Protocols.hpp
        Types.hpp
        TypeTraits.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        BlockVariables
)
