# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Wrappers")

set(LIBRARY_SOURCES
        Test_WithWrapper.cpp
        Test_AtWrapper.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Wrappers"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
