# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_H5_Interface")

set(LIBRARY_SOURCES
        Test_H5Metadata.cpp
        Test_H5Interface.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/IO/H5/Interface"
        "${LIBRARY_SOURCES}"
        "Utilities;H5Interface"
)
