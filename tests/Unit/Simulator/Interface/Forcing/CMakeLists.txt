# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Forcing")

set(LIBRARY_SOURCES
        Test_BatchPolicy.cpp
        Test_ForcerBase.cpp
        Test_Forcer.cpp
        Test_ForcingBase.cpp
        Test_Forcing.cpp
        Test_ForcingUUID.cpp
        Test_Serialize.cpp
        Test_ExecutionPolicy.cpp
        Test_Module.cpp
        )

# FIXME refactor serialization
add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Forcing"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator;Serialization"
)
