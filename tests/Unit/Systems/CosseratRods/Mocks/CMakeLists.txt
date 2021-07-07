set(LIBRARY "Test_CosseratRodMocks")

set(LIBRARY_SOURCES
        Test_CosseratRodTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods/Mocks"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Systems"
)
