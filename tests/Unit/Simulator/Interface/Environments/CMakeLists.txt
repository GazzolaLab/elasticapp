# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Environments")

set(LIBRARY_SOURCES
        Test_Emplacer.cpp
        Test_ExecutionPolicy.cpp
        Test_WithImposedPrimitives.cpp
        #        Test_Serialize.cpp
        Test_Module.cpp
        Test_WithPrimitives.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Environments"
        "${LIBRARY_SOURCES}"
        "ErrorHandling;Utilities;Primitives;Simulator"
)
