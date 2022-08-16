# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_GravityField")

set(LIBRARY_SOURCES
        Test_GravityField.cpp
        Test_ModuleConfiguration.cpp
        )

add_test_library(
        ${LIBRARY}
        "Simulator/Environments/GravityField"
        "${LIBRARY_SOURCES}"
        # Simulator is required for integrated tests.
        # FIXME : https://github.com/tp5uiuc/elasticapp/issues/228
        # Sanitize
        "GravityField;Simulator" 
)
