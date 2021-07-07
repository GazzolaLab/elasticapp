set(LIBRARY "Test_OrderTags")

set(LIBRARY_SOURCES
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/States/Expressions/OrderTags"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
