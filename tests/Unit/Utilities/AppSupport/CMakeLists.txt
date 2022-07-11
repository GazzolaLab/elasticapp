# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "Test_Utilities_AppSupport")

set(LIBRARY_SOURCES
        Test_CaseStudyInterface.cpp
        )

add_test_library(
        ${LIBRARY}
        "Utilities/AppSupport"
        "${LIBRARY_SOURCES}"
        "_elastica_case_study;_elastica_case_study_interface"
)

add_subdirectory(Helpers)