# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY InterfaceWrappers)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        AtWrapper.hpp
        QueryID.hpp
        ToWrapper.hpp
        UUID.hpp
        UUIDMap.hpp
        WithWrapper.hpp
        Wrappers.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        ${LIBRARY_SOURCES}
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Stl
        Utilities
)

set(LIBRARY InterfaceWrappersSerialization)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Serialize.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        ${LIBRARY_SOURCES}
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        InterfaceWrappers
        Serialization
)
