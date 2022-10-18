# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY BlockCore)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Aliases.hpp
        AsVariables.hpp
        Block.hpp
        BlockFacade.hpp
        BlockIterator.hpp
        BlockSlice.hpp
        BlockSliceFacade.hpp
        BlockView.hpp
        BlockViewFacade.hpp
        Concepts.hpp
        Metadata.hpp
        Protocols.hpp
        Types.hpp
        TypeTraits.hpp
        VariableCache.hpp
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

add_subdirectory(Concepts)
add_library(${LIBRARY}::Concepts ALIAS BlockConcepts)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        BlockVariables
        BlockConcepts
)
