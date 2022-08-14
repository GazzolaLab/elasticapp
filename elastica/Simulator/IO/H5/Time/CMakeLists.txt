# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY H5Time)

add_elastica_library(${LIBRARY})

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        Time.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        Time.cpp
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        HighFive
        Time
)
