set(LIBRARY "Test_Block")

set(LIBRARY_SOURCES
        Test_BlockFacade.cpp
        Test_BlockIterator.cpp
        # This test is disabled because slice now requires a block, so it becomes more of an integrated test.
        # So the tests in Test_BlocksFramework are retained, and the ones in the current folder are skipped.
        # Test_BlockSlice.cpp
        Test_Protocols.cpp
        Test_TypeTraits.cpp
        Test_Metadata.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/Block/Block"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
