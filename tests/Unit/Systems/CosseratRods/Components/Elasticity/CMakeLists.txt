add_subdirectory(detail)

set(LIBRARY "Test_CosseratRodElasticity")

set(LIBRARY_SOURCES
        Test_ExplicitDampingAdapter.cpp
        Test_WithDiagonalLinearHyperElasticModel.cpp
        Test_WithExplicitlyDampedDiagonalLinearHyperElasticModel.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Components/Elasticity"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
