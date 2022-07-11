set(LIBRARY "Test_SO3Expressions")

set(LIBRARY_SOURCES
        Test_SO3.cpp
        Test_SO3_blaze.cpp
        Test_SO3PrimitiveAddAssign.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/States/Expressions/SO3"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
