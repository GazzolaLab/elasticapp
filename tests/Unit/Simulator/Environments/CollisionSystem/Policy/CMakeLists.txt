# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_CollisionPolicy")

set(LIBRARY_SOURCES
        Test_ExecutionPolicy.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Policy"
        "${LIBRARY_SOURCES}"
        "CollisionPolicy;Primitives;Utilities"
)
