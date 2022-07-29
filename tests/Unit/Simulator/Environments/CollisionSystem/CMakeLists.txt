# Distributed under the MIT License.
# See LICENSE.txt for details.


add_subdirectory(Batches)
add_subdirectory(Contact)
add_subdirectory(Detection)
add_subdirectory(ModuleConfiguration)
add_subdirectory(Policy)
add_subdirectory(Response)

set(LIBRARY "Test_CollisionSystem")

set(LIBRARY_SOURCES
        Test_DEMSolver.cpp
        )

# FIXME refactor serialization
add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem"
        "${LIBRARY_SOURCES}"
        "CollisionSystem;Serialization"
)
