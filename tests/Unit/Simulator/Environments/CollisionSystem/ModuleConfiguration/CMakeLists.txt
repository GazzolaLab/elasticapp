# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_CollisionSystemModuleConfiguration")

set(LIBRARY_SOURCES
        Test_ModuleConfiguration.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/ModuleConfiguration"
        "${LIBRARY_SOURCES}"
        "CollisionSystemModuleConfiguration;Utilities"
)
