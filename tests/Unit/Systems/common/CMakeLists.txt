set(LIBRARY "Test_CommonSystems")

set(LIBRARY_SOURCES
        Test_Protocols.cpp
        Test_SymplecticStepperAdapter.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/common"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Systems"
)
