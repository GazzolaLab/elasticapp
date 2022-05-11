# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CollisionBatches)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(UnionFindExecutionPolicy)
add_library(${LIBRARY}::UnionFindExecutionPolicy ALIAS UnionFindExecutionPolicy)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Batches.hpp
        BatchContainerTrait.hpp
        BatchGenerators.hpp
        ContactTrait.hpp
        HybridBatch.hpp
        RigidBodyPrimitiveTrait.hpp
        SingleBatch.hpp
        Types.hpp
        UnionFind.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Batches.hpp
        BatchContainerTrait.hpp
        BatchGenerators.hpp
        ContactTrait.hpp
        HybridBatch.hpp
        RigidBodyPrimitiveTrait.hpp
        SingleBatch.hpp
        Types.hpp
        UnionFind.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        UnionFindExecutionPolicy
        Utilities
        Primitives
        Contact
)
