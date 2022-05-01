# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY RigidBodyCustomization)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Common)
add_subdirectory(Sphere)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        common.hpp
        Sphere.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PUBLIC
        common.hpp
        Sphere.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        RigidBodyCommonCustomization
        RigidBodySphereCustomization
)
