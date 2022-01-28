# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Affordances")

set(LIBRARY_SOURCES
        Test_EnableReset.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Affordances"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
