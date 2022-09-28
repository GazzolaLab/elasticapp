# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY InterfaceTimeSteppers)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        ExplicitSteppers.hpp
        Metadata.hpp
        SymplecticSteppers.hpp
        Types.hpp
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
        Time
        Utilities
        SystemsCommon
        Stl
)