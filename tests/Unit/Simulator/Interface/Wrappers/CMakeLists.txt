# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Interface_Wrappers")

set(LIBRARY_SOURCES
        Test_WithWrapper.cpp
        Test_AtWrapper.cpp
        Test_QueryID.cpp
        Test_UUID.cpp
        Test_UUIDMap.cpp
        Test_Serialize.cpp
        Test_ToWrapper.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Interface/Wrappers"
        "${LIBRARY_SOURCES}"
        "Utilities;Serialization"
)
