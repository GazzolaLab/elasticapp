# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY RigidBodySphereCustomization)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        PrimitiveTrait.hpp
        Transfer.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        PrimitiveTrait.hpp
        Transfer.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        RigidBodyCommonCustomization
        RigidBodySphereComponents
        Primitives
        Transfer
)
