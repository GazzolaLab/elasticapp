# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY EnvironmentsCommon)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Access.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Access.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Time
        Utilities
)


