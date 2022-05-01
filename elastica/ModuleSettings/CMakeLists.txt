# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY ModuleSettings)

add_elastica_library(${LIBRARY} INTERFACE)
add_library(${CMAKE_PROJECT_NAME}::ModuleSettings ALIAS ModuleSettings)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Collisions.hpp
        DEMSolver.hpp
        HashGrids.hpp
        Parallel.hpp
        Profiling.hpp
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
        Version.hpp
)

# Do customization based on compile-time options here
if (ELASTICA_SMP_THREADS STREQUAL "TBB")
    if (TBB_FOUND)
        target_compile_definitions(${LIBRARY} INTERFACE ELASTICA_USE_TBB)
        target_link_libraries(${LIBRARY} INTERFACE TBB)
    else ()
        message(WARNING "TBB not found. Elastica will running on a single thread. Try C++11.")
    endif ()
endif ()
