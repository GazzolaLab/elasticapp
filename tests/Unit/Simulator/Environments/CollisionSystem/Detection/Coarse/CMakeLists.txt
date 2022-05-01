# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_CoarseCollisionDetection")

set(LIBRARY_SOURCES
        Test_BoundingBox.cpp
        Test_ExhaustiveSearch.cpp
        Test_RigidBodyPrimitiveTrait.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Detection/Coarse"
        "${LIBRARY_SOURCES}"
        "CoarseCollisionDetection;ContactCache;CollisionPolicy;Primitives;Utilities"
)
