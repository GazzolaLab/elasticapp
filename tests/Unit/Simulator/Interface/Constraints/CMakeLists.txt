# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Constraints")

set(LIBRARY_SOURCES
        Test_Constraint.cpp
        Test_ConstrainerBase.cpp
        Test_Constrainer.cpp
        Test_ConstraintBase.cpp
        Test_ConstraintUUID.cpp
        Test_ExecutionPolicy.cpp
        Test_Serialize.cpp
        Test_Module.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Constraints"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
