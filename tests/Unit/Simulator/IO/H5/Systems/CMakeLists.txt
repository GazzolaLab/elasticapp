# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_H5_Systems")

set(LIBRARY_SOURCES
        Test_CosseratRod.cpp
        Test_Sphere.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/IO/H5/Systems"
        "${LIBRARY_SOURCES}"
        "Utilities;H5Systems"
)
