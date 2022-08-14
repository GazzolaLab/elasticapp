# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_IO_Interface")

set(LIBRARY_SOURCES
        Test_FileNamingConvention.cpp
        Test_Helpers.cpp
        Test_MetadataInterface.cpp
        Test_WorkingDirectory.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/IO/Interface"
        "${LIBRARY_SOURCES}"
        "IO_Interface"
)
