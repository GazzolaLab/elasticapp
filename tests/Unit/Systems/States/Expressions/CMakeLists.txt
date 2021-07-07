add_subdirectory(OrderTags)
add_subdirectory(States)
add_subdirectory(SO3)
add_subdirectory(SE3)

set(LIBRARY "Test_StatesIntegration")

set(LIBRARY_SOURCES
        Test_StatesWithSO3SE3.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/States/Expressions"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
