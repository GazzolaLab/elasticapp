# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Primitives)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        CapsuleFromRodPrimitive.cpp
        CapsulePrimitive.cpp
        CylinderPrimitive.cpp
        GeomPrimitive.cpp
        ImposedPrimitiveAdapter.hpp
        PlanePrimitive.cpp
        PrimitivesCollection.hpp
        RigidBodyPrimitive.cpp
        Serialize.hpp
        SpherePrimitive.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Access.hpp
        Aliases.hpp
        CapsuleFromRodPrimitive.hpp
        CapsulePrimitive.hpp
        CylinderPrimitive.hpp
        GeomPrimitive.hpp
        ImposedPrimitiveAdapter.hpp
        PlanePrimitive.hpp
        PrimitivesCollection.hpp
        Protocols.hpp
        RigidBodyPrimitive.hpp
        Serialize.hpp
        SpherePrimitive.hpp
        Types.hpp
        TypeTraits.hpp
)

add_subdirectory(detail)

#target_include_directories(${LIBRARY} Environments)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Frames
        Materials
        ErrorHandling
        Utilities
        # For imposed primitives
        Time
        Kinematics
)
