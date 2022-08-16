# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_FineCollisionDetection")

set(LIBRARY_SOURCES
        Test_FineDetectionInterface.cpp
        Test_CapsuleCapsule.cpp
        Test_CapsuleHalfSpace.cpp
        Test_CapsulePlane.cpp
        Test_SphereCapsule.cpp
        Test_SphereCylinder.cpp
        Test_SphereHalfSpace.cpp
        Test_SpherePlane.cpp
        Test_SphereSphere.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Detection/Fine"
        "${LIBRARY_SOURCES}"
        "FineCollisionDetection;ContactCache;CollisionPolicy;Utilities"
)
