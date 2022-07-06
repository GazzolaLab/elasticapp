# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SerializationCustom)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        blaze.hpp
        TaggedTuple.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Blaze
        Utilities
        ErrorHandling
)
