# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_AppSupport_Helpers")

set(LIBRARY_SOURCES
        Test_MultipleJustGreaterThan.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/AppSupport/Helpers"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
