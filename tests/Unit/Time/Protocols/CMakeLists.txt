set(LIBRARY "Test_TimeProtocols")

set(LIBRARY_SOURCES
        Test_SymplecticSteppable.cpp
        )

add_test_library(
        ${LIBRARY}
        "Time/Protocols"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Time"
)
