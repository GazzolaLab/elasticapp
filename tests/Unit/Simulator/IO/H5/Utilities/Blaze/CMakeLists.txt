# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_H5_Utilities_Blaze")

set(LIBRARY_SOURCES
        Test_Subvector.cpp
        Test_Submatrix.cpp
        Test_Subtensor.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/IO/H5/Utilities/Blaze"
        "${LIBRARY_SOURCES}"
        "H5UtilitiesBlaze;Utilities"
)
