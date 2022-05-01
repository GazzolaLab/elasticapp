set(LIBRARY "Test_DEMForces")

set(LIBRARY_SOURCES
        Test_LinearSpringDashpot.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/CollisionSystem/Response/DEMForces"
        "${LIBRARY_SOURCES}"
        "DEMForces"
)
