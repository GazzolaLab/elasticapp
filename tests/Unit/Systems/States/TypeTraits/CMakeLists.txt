set(LIBRARY "Test_TypeTraits")

set(LIBRARY_SOURCES
        Test_HasOrder.cpp
        Test_IsExpression.cpp
        Test_IsPrimitive.cpp
        Test_IsVectorized.cpp
        Test_SupportsVectorizedOperations.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/States/TypeTraits"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Systems"
)
