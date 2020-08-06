# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Domain)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        Domain.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Domain.hpp
        Span.hpp
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        ErrorHandling
        Utilities
        Blaze
)
