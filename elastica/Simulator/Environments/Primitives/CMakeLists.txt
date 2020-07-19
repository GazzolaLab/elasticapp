# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Primitives)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        RigidBodyPrimitive.cpp
        GeomPrimitive.cpp
        SpherePrimitive.cpp
        PlanePrimitive.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        RigidBodyPrimitive.hpp
        GeomPrimitive.hpp
        SpherePrimitive.hpp
        PlanePrimitive.hpp
)

#target_include_directories(${LIBRARY} Environments)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Simulator # For materials
        ErrorHandling
        Utilities
)
