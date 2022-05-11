# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SystemsCustomization)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(CosseratRods)
add_library(${LIBRARY}::CosseratRods ALIAS CosseratRodsCustomization)

add_subdirectory(RigidBody)
add_library(${LIBRARY}::RigidBody ALIAS RigidBodyCustomization)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        CosseratRods.hpp
        RigidBody.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        CosseratRods.hpp
        RigidBody.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CosseratRodsCustomization
        RigidBodyCustomization
)
