set(LIBRARY "Test_Constraints")

set(LIBRARY_SOURCES
        Test_Constraint.cpp
        Test_MakeConstraint.cpp
        Test_Registration.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Constraints"
        "${LIBRARY_SOURCES}"
        "Utilities;Simulator"
)
