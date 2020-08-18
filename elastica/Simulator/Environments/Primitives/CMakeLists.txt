# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Primitives)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        ApproximateCylinderPrimitive.cpp
        RigidBodyPrimitive.cpp
        CapsulePrimitive.cpp
        CylinderPrimitive.cpp
        GeomPrimitive.cpp
        GeomTools.cpp
        SpherePrimitive.cpp
        PlanePrimitive.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ApproximateCylinderPrimitive.hpp
        CapsulePrimitive.hpp
        ContiguousPrimitives.hpp
        CylinderPrimitive.hpp
        RigidBodyPrimitive.hpp
        GeomPrimitive.hpp
        GeomTools.hpp
        SpherePrimitive.hpp
        PlanePrimitive.hpp
)

add_subdirectory(detail)

#target_include_directories(${LIBRARY} Environments)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Simulator # For materials
        ErrorHandling
        Utilities
)
