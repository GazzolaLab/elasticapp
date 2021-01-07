# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_NamedType")

set(LIBRARY_SOURCES
        Test_Affordances.cpp
        Test_NamedType.cpp
        Test_MakeNamed.cpp
        Test_Traits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/NamedType"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
