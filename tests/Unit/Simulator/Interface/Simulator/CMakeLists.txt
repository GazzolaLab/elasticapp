# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Simulator")

set(LIBRARY_SOURCES
        Test_Simulator.cpp
        Test_SystemsRegistrationHelpers.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Simulator"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
