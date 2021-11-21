set(LIBRARY "Test_CosseratRod_FullRodTest")

set(LIBRARY_SOURCES
        Test_PerturbedRod.cpp
        Test_PerturbedRodWithNoStep.cpp
        Test_PerturbedRodWithOneStep.cpp
        Test_PerturbedRodWithThreeStep.cpp
        Test_PerturbedTwoRodWithThreeStep.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
