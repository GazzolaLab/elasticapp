# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Batching")

set(LIBRARY_SOURCES
        Test_BatchingHelpers.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Batching"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
