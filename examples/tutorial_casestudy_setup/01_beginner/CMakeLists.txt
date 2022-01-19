add_library(tutorial_beginner_casestudy STATIC tutorial_beginner.cpp)
target_sources(tutorial_beginner_casestudy INTERFACE tutorial_beginner.hpp)

target_link_libraries(tutorial_beginner_casestudy
        PUBLIC
        Utilities
        ErrorHandling
        ElasticaFlags
        )

include(ElasticaCaseStudyHelpers)
add_elastica_case_study(StandaloneBeginnerCaseStudy
        FROM tutorial_beginner_casestudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyBeginnerCaseStudy
        FROM tutorial_beginner_casestudy
        AS elastica::python_object)
