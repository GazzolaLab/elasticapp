
set(LIBRARY "Test_CollisionBatches")

set(LIBRARY_SOURCES
        Test_SingleBatch.cpp
        Test_HybridBatch.cpp
        Test_UnionFind.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Batches"
        "${LIBRARY_SOURCES}"
        "CollisionBatches"
)
