set(LIBRARY "Test_CosseratRodElasticityDetail")

set(LIBRARY_SOURCES
        Test_ElasticityInterface.cpp
        Test_LinearHyperElasticityFacade.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Components/Elasticity/detail"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
