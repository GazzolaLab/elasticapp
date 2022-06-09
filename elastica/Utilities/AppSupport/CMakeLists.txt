# Distributed under the MIT License.
# See LICENSE.txt for details.

add_library(_elastica_case_study INTERFACE)
elastica_target_headers(_elastica_case_study
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        CaseStudy.hpp)
target_link_libraries(_elastica_case_study INTERFACE Utilities)

add_library(_elastica_standalone_support INTERFACE)
elastica_target_headers(_elastica_standalone_support
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        CaseStudyParser.hpp)
target_link_libraries(_elastica_standalone_support INTERFACE Utilities _elastica_case_study)

# to be customized based on build/lib so that we can directly call headers here
# TODO : Is this a good idea?
#target_include_directories(_elastica_standalone_support INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})


# pybindings
add_subdirectory(Helpers)
add_subdirectory(Python)