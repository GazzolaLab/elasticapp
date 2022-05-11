# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY RigidBodyAliases)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        RigidBodyComponents
        RigidBodySphereComponents
        RigidBodyTraits
)
