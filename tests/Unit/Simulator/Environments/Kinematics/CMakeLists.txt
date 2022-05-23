# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Kinematics")

set(LIBRARY_SOURCES
        Test_Translations.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/Kinematics"
        "${LIBRARY_SOURCES}"
        "Kinematics"
)
