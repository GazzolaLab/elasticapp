set(LIBRARY "Test_Configuration_Defaults")
set(LIBRARY_SOURCES
        Test_Domain.cpp
        Test_Connections.cpp
        Test_Constraints.cpp
        Test_Environments.cpp
        Test_Forcing.cpp
        Test_Systems.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Configuration/Defaults"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
