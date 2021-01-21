# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Indexers")

set(LIBRARY_SOURCES
        Test_At.cpp
        Test_Range.cpp
        Test_Pair.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Indexers"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
