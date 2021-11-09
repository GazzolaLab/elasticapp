# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_Math")

set(LIBRARY_SOURCES
        Test_Normalize.cpp
        Test_Orthogonal.cpp
        Test_Zero.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/Math"
        "${LIBRARY_SOURCES}"
        "Utilities;Blaze"
)
