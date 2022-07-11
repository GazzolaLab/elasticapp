add_library(tutorial_intermediate_casestudy STATIC tutorial_intermediate.cpp)
target_sources(tutorial_intermediate_casestudy INTERFACE tutorial_intermediate.hpp)

target_link_libraries(tutorial_intermediate_casestudy
        PUBLIC
        Utilities
        ErrorHandling
        ElasticaFlags
        Blaze
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(tutorial_intermediate_casestudy PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)
add_elastica_case_study(StandaloneIntermediateCaseStudy
        FROM tutorial_intermediate_casestudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyIntermediateCaseStudy
        FROM tutorial_intermediate_casestudy
        AS elastica::python_object)

elastica_python_link_libraries(PyIntermediateCaseStudy PRIVATE Blaze Utilities)
elastica_python_add_dependencies(PyIntermediateCaseStudy PyArrays)
