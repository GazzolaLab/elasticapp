# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Kinematics)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Translations.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Translations.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Frames
        Utilities
        Time
)


