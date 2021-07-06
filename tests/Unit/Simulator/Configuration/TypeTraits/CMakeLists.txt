set(LIBRARY "Test_Configuration_TypeTraits")
set(LIBRARY_SOURCES
        #        Test_BlockingPolicy.cpp
        #        Test_common.cpp
        Test_Domain.cpp
        #        Test_ExecutionPolicy.cpp
        #        Test_IterationPolicy.cpp
        Test_Connections.cpp
        Test_Constraints.cpp
        Test_Forcing.cpp
        Test_Environments.cpp
        Test_Systems.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Configuration/TypeTraits"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
