set(LIBRARY "Test_DEMExecutionPolicy")

set(LIBRARY_SOURCES
        Test_SequentialContactResolver.cpp
        Test_ParallelContactResolver.cpp
        Test_SequentialExecutionPolicy.cpp
        Test_ParallelExecutionPolicy.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Response/DEMExecutionPolicy"
        "${LIBRARY_SOURCES}"
        "DEMExecutionPolicy"
)
