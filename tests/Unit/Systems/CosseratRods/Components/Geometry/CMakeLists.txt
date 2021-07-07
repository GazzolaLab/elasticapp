add_subdirectory(detail)

set(LIBRARY "Test_CosseratRodGeometry")

set(LIBRARY_SOURCES
        Test_WithCircleCrossSection.cpp
        Test_WithSquareCrossSection.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Components/Geometry"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
