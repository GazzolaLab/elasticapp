# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_TimeSteppers")

set(LIBRARY_SOURCES
        Test_SymplecticSteppers.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/TimeSteppers"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
