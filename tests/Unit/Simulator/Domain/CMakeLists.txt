set(LIBRARY "Test_Domain")
set(LIBRARY_SOURCES
        Test_BoundaryAlong.cpp
        Test_Span.cpp
        Test_Domain.cpp
        Test_Serialize.cpp
        Test_TypeTraits.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Domain"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
