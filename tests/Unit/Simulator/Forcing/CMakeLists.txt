set(LIBRARY "Test_Forcing")

set(LIBRARY_SOURCES
        Test_Forcing.cpp
        Test_MakeForcing.cpp
        Test_Registration.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Forcing"
        "${LIBRARY_SOURCES}"
        "Utilities;Simulator"
)
