# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY H5Utilities)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Blaze)

set(LIBRARY_SOURCES
        Blaze.hpp
        TaggedTuple.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
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
        H5UtilitiesBlaze
        Utilities
        HighFive
        Stl
)
