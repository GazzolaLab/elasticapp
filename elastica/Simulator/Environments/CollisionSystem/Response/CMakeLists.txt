# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CollisionResponse)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(DEMForces)
add_library(${LIBRARY}::DEMForces ALIAS DEMForces)

add_subdirectory(DEMExecutionPolicy)
add_library(${LIBRARY}::DEMExecutionPolicy ALIAS DEMExecutionPolicy)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ContactTrait.hpp
        DEMSolver.hpp
        Protocols.hpp
        Response.hpp
        RigidBodyPrimitiveTrait.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PUBLIC
        ContactTrait.hpp
        DEMSolver.hpp
        Protocols.hpp
        Response.hpp
        RigidBodyPrimitiveTrait.hpp
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        DEMForces
        DEMExecutionPolicy
        Utilities
)
