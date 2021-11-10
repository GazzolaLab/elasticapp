# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Materials")

set(LIBRARY_SOURCES
        Test_Materials.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Materials"
        "${LIBRARY_SOURCES}"
        "Materials"
)
