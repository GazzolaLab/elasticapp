set(LIBRARY "Test_Frames")
set(LIBRARY_SOURCES
        Test_EulerianFrame.cpp
        Test_LagrangianFrame.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Frames"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
