# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY ModuleSettings)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Collisions.hpp
        DEMSolver.hpp
        HashGrids.hpp
        Parallel.hpp
        Profiling.hpp
        Systems.hpp
        Vectorization.hpp
        Version.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)
