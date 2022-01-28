add_subdirectory(Affordances)
add_subdirectory(Batching)
add_subdirectory(Connections)
add_subdirectory(Constraints)
add_subdirectory(Forcing)
add_subdirectory(Systems)
add_subdirectory(Wrappers)
add_subdirectory(Indexers)
add_subdirectory(TimeSteppers)
add_subdirectory(Simulator)

set(LIBRARY "Test_Interface")

set(LIBRARY_SOURCES
        Test_MockSimulator.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
