set(LIBRARY "Test_CosseratRod_Traits_PlacementTraits")

set(LIBRARY_SOURCES
        Test_Protocols.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Traits/PlacementTraits"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Systems"
)
