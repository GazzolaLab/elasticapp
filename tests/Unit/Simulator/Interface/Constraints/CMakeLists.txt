# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Constraints")

set(LIBRARY_SOURCES
        Test_Constraint.cpp
        Test_ConstraintBase.cpp
        Test_ConstraintUUID.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Constraints"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
