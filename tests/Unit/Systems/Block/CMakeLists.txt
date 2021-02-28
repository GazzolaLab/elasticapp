add_subdirectory(Block)
add_subdirectory(BlockVariables)

set(LIBRARY "Test_BlocksFramework")

set(LIBRARY_SOURCES
        Test_BlocksFramework.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/Block"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
