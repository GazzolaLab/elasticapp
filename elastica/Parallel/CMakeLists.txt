# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY Parallel)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        blocked_range.hpp
        detail.hpp
        for_each.hpp
        invoke.hpp
        reduce.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        # Make this link to TBB
        ModuleSettings
)

#if (ELASTICA_SMP)
#    target_link_libraries(
#            ${LIBRARY}
#            INTERFACE
#            Utilities
#            TBB
#    )
#else ()
#    target_link_libraries(
#            ${LIBRARY}
#            INTERFACE
#            Utilities
#    )
#endif ()
