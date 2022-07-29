# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_Timing")

set(LIBRARY_SOURCES
        Test_Timer.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/Timing"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
