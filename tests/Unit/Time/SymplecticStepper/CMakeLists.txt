set(LIBRARY "Test_Time_SymplecticStepper")

set(LIBRARY_SOURCES
        Test_SymplecticStepper.cpp
        )

add_test_library(
        ${LIBRARY}
        "Time/SymplecticStepper"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Time"
)
