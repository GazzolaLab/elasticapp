add_subdirectory(Helpers)
add_subdirectory(SymplecticStepper)
add_subdirectory(Protocols)

set(LIBRARY "Test_Time")

set(LIBRARY_SOURCES
        Test_SimulationTime.cpp
        Test_TimeStepID.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Time"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Time;Serialization"
)
