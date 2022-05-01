# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_Geometry")

set(LIBRARY_SOURCES
        Test_Angles.cpp
        Test_AxisAngleRotation.cpp
        Test_GetClosestLineSegmentPoints.cpp
        Test_GetClosestPointOnLine.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/Geometry"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
