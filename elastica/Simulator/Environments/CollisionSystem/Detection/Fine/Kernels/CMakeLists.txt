set(LIBRARY FineCollisionDetectionKernels)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        CapsuleCapsule.hpp
        CapsuleHalfSpace.hpp
        CapsulePlane.hpp
        ConvexConvex.hpp
        RodCapsuleRodCapsule.hpp
        SphereSphere.hpp
        SphereCapsule.hpp
        SphereHalfSpace.hpp
        SpherePlane.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Contact
        Primitives
        Utilities
)
