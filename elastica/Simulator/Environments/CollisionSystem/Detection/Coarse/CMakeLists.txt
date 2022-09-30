###############################################################################

set(LIBRARY CoarseCollisionDetectionCore)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        BoundingBox.hpp
        ContactTrait.hpp
        RigidBodyPrimitiveTrait.hpp
        Types.hpp
        TypeTraits.hpp
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
        Frames # BoundingBox
        ErrorHandling
        Utilities
        Primitives # PrimitiveTrait
        Stl # cmath
)

###############################################################################

set(LIBRARY CoarseCollisionDetectionExhaustiveSearch)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        ExhaustiveSearch.hpp
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
        CoarseCollisionDetectionCore
        CollisionExecutionPolicy
)

###############################################################################

set(LIBRARY CoarseCollisionDetectionHashGrids)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        HashGrids.hpp
        HashGridPrimitiveCache.hpp
        HashGridsConfigurationParameters.hpp
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

add_subdirectory(HashGridPolicies)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CoarseCollisionDetectionCore
        CollisionExecutionPolicy
        HashGridPolicies
        ModuleSettings
)

###############################################################################
set(LIBRARY CoarseCollisionDetection)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        Detectors.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Detectors.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CoarseCollisionDetectionCore
        CoarseCollisionDetectionExhaustiveSearch
        CoarseCollisionDetectionHashGrids
        Contact
)

