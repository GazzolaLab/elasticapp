# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY Framework)

add_subdirectory(Tests)

add_elastica_library(${LIBRARY} INTERFACE)

set(STRING_LIBRARY StringAlgorithm)
set(STRING_LIBRARY_SOURCES
        StringAlgorithm.cpp
        )
add_library(${STRING_LIBRARY} ${STRING_LIBRARY_SOURCES})

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Options
        Parallel
        Utilities
        # PUBLIC
        ErrorHandling
        ${STRING_LIBRARY}
)
