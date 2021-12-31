add_library(tutorial_casestudy_setup STATIC tutorial_beginner.cpp)
target_sources(tutorial_casestudy_setup INTERFACE tutorial_beginner.hpp)

target_link_libraries(tutorial_casestudy_setup
        PUBLIC
        Utilities
        ErrorHandling
        ElasticaFlags
        )

include(ElasticaCaseStudyHelpers)
add_elastica_case_study(standalone_tutorial_casestudy_setup
        FROM tutorial_casestudy_setup
        AS elastica::standalone_executable)

add_elastica_case_study(py_tutorial_casestudy_setup
        FROM tutorial_casestudy_setup
        AS elastica::python_object)
