add_subdirectory(Mocks)
add_subdirectory(Components)
add_subdirectory(Initializers)
add_subdirectory(Traits)

set(LIBRARY "Test_CosseratRod")

set(LIBRARY_SOURCES
        Test_CosseratRodPlugin.cpp
        Test_BlockInitializer.cpp
        Test_Block.cpp
        Test_Access.cpp
        Test_Serialize.cpp
        Test_Specialize.cpp
        Test_Tagged.cpp
        Test_Utilities.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/CosseratRods"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
