# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_BlazeMath")

set(LIBRARY_SOURCES
        Test_BlazeCalculus.cpp
        Test_BlazeLinearAlgebra.cpp
        Test_BlazeRotation.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/Math/BlazeDetail"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
