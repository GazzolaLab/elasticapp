# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Connections")

set(LIBRARY_SOURCES
        Test_Connecter.cpp
        Test_ConnecterBase.cpp
        Test_Connection.cpp
        Test_ConnectionBase.cpp
        Test_ConnectionUUID.cpp
        Test_ExecutionPolicy.cpp
        Test_Serialize.cpp
        Test_Module.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Connections"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Simulator"
)
