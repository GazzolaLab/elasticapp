set(LIBRARY "Test_CosseratRodCustomization")

set(LIBRARY_SOURCES
        Test_PrimitiveTrait.cpp
        Test_Transfer.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/Customization/CosseratRods"
        "${LIBRARY_SOURCES}"
        "CosseratRodsCustomization;Primitives"
)
