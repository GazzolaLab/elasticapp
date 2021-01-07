add_subdirectory(Constraints)
add_subdirectory(Wrappers)

set(LIBRARY "Test_Interface")

set(LIBRARY_SOURCES
        Test_At.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface"
        "${LIBRARY_SOURCES}"
        "Utilities"
)
