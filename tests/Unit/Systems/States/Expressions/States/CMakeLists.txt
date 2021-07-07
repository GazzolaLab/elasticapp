set(LIBRARY "Test_ExpressionsState")

set(LIBRARY_SOURCES
        Test_State.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/States/Expressions/States"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
