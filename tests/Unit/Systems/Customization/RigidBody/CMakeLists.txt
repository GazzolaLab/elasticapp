set(LIBRARY "Test_RigidBodyCustomization")

set(LIBRARY_SOURCES
        Test_Sphere.cpp
        )

# FIXME refactor serialization
add_test_library(
        ${LIBRARY}
        "Systems/Customization/RigidBody"
        "${LIBRARY_SOURCES}"
        "RigidBodyCustomization;Primitives"
)
