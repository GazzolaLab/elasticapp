# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_FineCollisionDetection")

set(LIBRARY_SOURCES
        Test_FineDetectionInterface.cpp
        Test_MaxContacts.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Detection/Fine"
        "${LIBRARY_SOURCES}"
        "FineCollisionDetection;ContactCache;CollisionPolicy;Utilities"
)
