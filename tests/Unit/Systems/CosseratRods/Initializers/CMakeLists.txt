set(LIBRARY "Test_CosseratRodInitializers")

set(LIBRARY_SOURCES
        Test_Affordances.cpp
        Test_ComposeInitializers.cpp
        Test_Elasticity.cpp
        Test_Geometry.cpp
        Test_StraightCosseratRod.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Initializers"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
