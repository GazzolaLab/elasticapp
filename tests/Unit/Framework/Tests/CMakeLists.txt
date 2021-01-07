# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Framework")

set(LIBRARY_SOURCES
        Test_TestCreation.cpp
        Test_TestingFramework.cpp
        )

add_test_library(
        ${LIBRARY}
        "Framework/Tests"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Options;StringAlgorithm"
)

# string algorithm is a bit hacky here, but I dont want boost