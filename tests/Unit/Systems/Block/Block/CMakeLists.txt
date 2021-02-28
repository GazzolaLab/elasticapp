set(LIBRARY "Test_Block")

set(LIBRARY_SOURCES
        Test_BlockFacade.cpp
        Test_BlockSlice.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/Block/Block"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
