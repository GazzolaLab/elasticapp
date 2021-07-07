set(LIBRARY "Test_SE3Expressions")

set(LIBRARY_SOURCES
        Test_SE3.cpp
        Test_SE3_blaze.cpp
        )

add_test_library(
        ${LIBRARY}
        "Systems/States/Expressions/SE3"
        "${LIBRARY_SOURCES}"
        "Utilities;Systems"
)
