# Distributed under the MIT License.
# See LICENSE.txt for details.

add_subdirectory(DEMForces)
add_subdirectory(DEMExecutionPolicy)

set(LIBRARY "Test_CollisionResponse")

set(LIBRARY_SOURCES
        Test_DEMSolver.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Response"
        "${LIBRARY_SOURCES}"
        "CollisionResponse;CollisionBatches;Contact"
)
