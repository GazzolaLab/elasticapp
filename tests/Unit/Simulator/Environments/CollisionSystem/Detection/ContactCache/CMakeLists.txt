# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_ContactCache")

set(LIBRARY_SOURCES
        Test_ContactCache.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Detection/ContactCache"
        "${LIBRARY_SOURCES}"
        "ContactCache;Utilities"
)
