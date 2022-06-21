# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_ConvertCase")

set(LIBRARY_SOURCES
        Test_ConvertCase.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/ConvertCase"
        "${LIBRARY_SOURCES}"
        "ConvertCase"
)
