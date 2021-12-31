# Distributed under the MIT License.
# See LICENSE.txt for details.

add_library(_elastica_case_study INTERFACE)
target_sources(_elastica_case_study INTERFACE CaseStudy.hpp)
target_link_libraries(_elastica_case_study INTERFACE Utilities)

add_library(_elastica_standalone_support INTERFACE)
target_sources(_elastica_standalone_support INTERFACE CaseStudyParser.hpp)
target_link_libraries(_elastica_standalone_support INTERFACE Utilities _elastica_case_study)

# to be customized based on build/lib
target_include_directories(_elastica_standalone_support INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})