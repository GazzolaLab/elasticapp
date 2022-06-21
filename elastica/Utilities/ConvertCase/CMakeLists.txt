# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY ConvertCase)

add_elastica_library(${LIBRARY})

elastica_target_sources(${LIBRARY}
        PRIVATE
        ConvertCase.cpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        ConvertCase.hpp
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        ElasticaFlags
        # Stl, include after PR goes through
)
