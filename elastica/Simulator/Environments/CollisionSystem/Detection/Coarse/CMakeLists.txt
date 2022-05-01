set(LIBRARY CoarseCollisionDetection)

add_elastica_library(${LIBRARY} INTERFACE)

# add_subdirectory(HashGridPolicies)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        BoundingBox.hpp
        ContactTrait.hpp
        Detectors.hpp
        ExhaustiveSearch.hpp
        # HashGridPrimitiveCache.hpp
        # HashGrids.hpp
        RigidBodyPrimitiveTrait.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        BoundingBox.hpp
        ContactTrait.hpp
        Detectors.hpp
        ExhaustiveSearch.hpp
        # HashGridPrimitiveCache.hpp
        # HashGrids.hpp
        RigidBodyPrimitiveTrait.hpp
        Types.hpp
        TypeTraits.hpp
)


target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Primitives
        Contact
        ErrorHandling
        Utilities
        CollisionExecutionPolicy
)
