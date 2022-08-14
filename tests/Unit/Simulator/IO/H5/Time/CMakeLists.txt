# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_H5_Time")

set(LIBRARY_SOURCES
        Test_Time.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/IO/H5/Time"
        "${LIBRARY_SOURCES}"
        "Utilities;H5Time"
)
