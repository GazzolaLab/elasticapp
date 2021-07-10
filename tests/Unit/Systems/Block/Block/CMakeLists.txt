set(LIBRARY "Test_Block")

set(LIBRARY_SOURCES
        Test_BlockFacade.cpp
        Test_BlockIterator.cpp
        Test_BlockSlice.cpp
        Test_Protocols.cpp
        Test_TypeTraits.cpp
        Test_DisplayVariables.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/Block/Block"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
