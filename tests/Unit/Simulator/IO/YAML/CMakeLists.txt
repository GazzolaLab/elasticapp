# Distributed under the MIT License.
# See LICENSE.txt for details.

if (TARGET YAMLInterface)
    set(LIBRARY "Test_YAML_Interface")

    set(LIBRARY_SOURCES
            Test_YAMLMetadata.cpp
            )

    add_test_library(
            ${LIBRARY}
            "Simulator/IO/YAML"
            "${LIBRARY_SOURCES}"
            "Utilities;YAMLInterface"
    )
endif ()
