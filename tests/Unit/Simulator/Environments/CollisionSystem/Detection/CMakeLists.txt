# Distributed under the MIT License.
# See LICENSE.txt for details.


add_subdirectory(Coarse)
add_subdirectory(ContactCache)
add_subdirectory(Fine)

set(LIBRARY "Test_CollisionDetection")

set(LIBRARY_SOURCES
        Test_PairwiseTMPL.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Detection"
        "${LIBRARY_SOURCES}"
        "CollisionDetection"
)
