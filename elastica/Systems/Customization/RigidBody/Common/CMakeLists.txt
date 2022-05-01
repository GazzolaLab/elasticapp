# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY RigidBodyCommonCustomization)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        PrimitiveTrait.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PUBLIC
        PrimitiveTrait.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        RigidBody
        SystemsCommon
        Primitives
)