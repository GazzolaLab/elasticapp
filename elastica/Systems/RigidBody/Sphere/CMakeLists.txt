# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY RigidBodySphereComponents)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Tags.hpp
        Types.hpp
        Sphere.hpp
        WithSphereGeometry.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Tags.hpp
        Types.hpp
        Sphere.hpp
        WithSphereGeometry.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        RigidBodyComponents
)

