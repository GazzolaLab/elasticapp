add_subdirectory(ModuleConfiguration)
add_subdirectory(Defaults)
add_subdirectory(TypeTraits)

set(LIBRARY "Test_Configuration")
set(LIBRARY_SOURCES
        Test_SimulatorConfiguration.cpp
        Test_DefaultSimulatorConfiguration.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Configuration"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator;Serialization" # FIXME refactor serialization
)
