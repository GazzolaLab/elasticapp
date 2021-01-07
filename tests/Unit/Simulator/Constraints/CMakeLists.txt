set(LIBRARY "Test_Constraints")

set(LIBRARY_SOURCES
        Test_MakeConstraint.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Constraints"
        "${LIBRARY_SOURCES}"
        "Utilities;Simulator"
)
