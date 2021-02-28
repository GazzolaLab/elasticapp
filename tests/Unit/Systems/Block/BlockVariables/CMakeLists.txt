set(LIBRARY "Test_BlockVariables")

set(LIBRARY_SOURCES
        Test_BlockInitializer.cpp
        Test_BlockVariables.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/Block/BlockVariables"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
