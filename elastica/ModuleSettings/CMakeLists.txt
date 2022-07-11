# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY ModuleSettings)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Collisions.hpp
        DEMSolver.hpp
        HashGrids.hpp
        Parallel.hpp
        Profiling.hpp
        Systems.hpp
        Version.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Collisions.hpp
        DEMSolver.hpp
        HashGrids.hpp
        Parallel.hpp
        Profiling.hpp
        Systems.hpp
        Version.hpp
)
