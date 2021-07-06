set(LIBRARY "Test_Connections")

set(LIBRARY_SOURCES
        Test_Connection.cpp
        Test_MakeConnection.cpp
        Test_Registration.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Connections"
        "${LIBRARY_SOURCES}"
        "Utilities;Simulator"
)
