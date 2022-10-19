# Distributed under the MIT License.
# See LICENSE.txt for details.

add_library(_elastica_python_support INTERFACE)
elastica_target_headers(_elastica_python_support
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        CaseStudyPybind.hpp
        PythonBindingHelpers.hpp)
target_link_libraries(_elastica_python_support
        INTERFACE
        AppSupportHelpers
        Utilities
        _elastica_case_study
        _elastica_case_study_interface)
