# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Serialization")

set(LIBRARY_SOURCES
        Test_Serialize.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/IO/Serialization"
        "${LIBRARY_SOURCES}"
        "Utilities;ErrorHandling;Serialization"
)
