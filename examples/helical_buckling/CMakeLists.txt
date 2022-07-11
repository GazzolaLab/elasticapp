add_library(HelicalBucklingCaseStudy STATIC HelicalBucklingCaseStudy.cpp)
target_sources(HelicalBucklingCaseStudy INTERFACE HelicalBucklingCaseStudy.hpp)

target_link_libraries(HelicalBucklingCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(HelicalBucklingCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_helical_buckling
        FROM HelicalBucklingCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyHelicalBucklingCaseStudy
        FROM HelicalBucklingCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyHelicalBucklingCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyHelicalBucklingCaseStudy PyArrays)
