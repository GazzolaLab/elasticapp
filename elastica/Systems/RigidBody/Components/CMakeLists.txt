# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY RigidBodyComponents)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        Tags.hpp
        Types.hpp
        WithRigidBodyGeometry.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Tags.hpp
        Types.hpp
        WithRigidBodyGeometry.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
)
