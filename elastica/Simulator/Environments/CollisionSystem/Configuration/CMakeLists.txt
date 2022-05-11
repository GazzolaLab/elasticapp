# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CollisionSystemConfiguration)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Default.hpp
        DEMSolver.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Default.hpp
        DEMSolver.hpp
        Types.hpp
)

# only needs types, still
target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        Primitives
        Contact
)
