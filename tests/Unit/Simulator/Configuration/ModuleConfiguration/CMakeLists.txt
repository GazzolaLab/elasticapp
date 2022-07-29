set(LIBRARY "Test_Configuration_ModuleConfiguration")
set(LIBRARY_SOURCES
        Test_BlockingPolicy.cpp
        Test_common.cpp
        Test_Domain.cpp
        Test_ExecutionPolicy.cpp
        Test_IterationPolicy.cpp
        Test_Connections.cpp
        Test_Constraints.cpp
        Test_Environments.cpp
        Test_Forcing.cpp
        Test_Parallel.cpp
        Test_Serialize.cpp
        Test_Systems.cpp
        )

# FIXME refactor serialization
add_test_library(
        ${LIBRARY}
        "Simulator/Configuration/ModuleConfiguration"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator;Serialization"
)
