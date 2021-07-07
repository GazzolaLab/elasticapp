set(LIBRARY "Test_Time_Helpers")

set(LIBRARY_SOURCES
        Test_Update.cpp
        )

add_test_library(
        ${LIBRARY}
        "Time/Helpers"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Time"
)
