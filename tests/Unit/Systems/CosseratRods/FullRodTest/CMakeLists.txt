set(LIBRARY "Test_CosseratRod_FullRodTest")

set(LIBRARY_SOURCES
        Test_PerturbedRod.cpp
        Test_PerturbedRodStep.cpp
        Test_PerturbedRodStepWithoutDamping.cpp
        Test_RodAPI.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/FullRodTest"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
