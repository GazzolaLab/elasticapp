set(LIBRARY "Test_CosseratRodGeometryDetail")

set(LIBRARY_SOURCES
        Test_RodSpanwiseGeometry.cpp
        Test_CosseratRodSpanwiseGeometry.cpp
        Test_CosseratRodCrossSectionInterface.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Components/Geometry/detail"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
