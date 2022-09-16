set(LIBRARY "Test_RigidBody")

set(LIBRARY_SOURCES
        Test_RigidBodyAPI.cpp
        Test_RigidBodyPlugin.cpp
        Test_Sphere.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/RigidBody"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
