set(LIBRARY "Test_CosseratRodComponentsHelpers")

set(LIBRARY_SOURCES
        Test_AdapterHelpers.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Components/helpers"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
