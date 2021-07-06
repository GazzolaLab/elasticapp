# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Systems")

set(LIBRARY_SOURCES
        # Test_Serialize.cpp
        Test_ExecutionPolicy.cpp
        Test_Module.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Systems"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
