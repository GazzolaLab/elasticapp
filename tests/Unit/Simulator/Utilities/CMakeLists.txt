# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_SimulatorUtilities")

set(LIBRARY_SOURCES
        Test_MemoryMeters.cpp
        Test_Offsets.cpp
        Test_Timers.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Utilities"
        "${LIBRARY_SOURCES}"
        "SimulatorUtilities"
)
