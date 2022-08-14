# Distributed under the MIT License.
# See LICENSE.txt for details.

add_subdirectory(Blaze)

set(LIBRARY "Test_H5_Utilities")

set(LIBRARY_SOURCES
        Test_TaggedTuple.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/IO/H5/Utilities"
        "${LIBRARY_SOURCES}"
        "Utilities;H5Utilities"
)
