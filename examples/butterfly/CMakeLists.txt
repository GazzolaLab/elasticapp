add_library(ButterflyCaseStudy STATIC ButterflyCaseStudy.cpp)
target_sources(ButterflyCaseStudy INTERFACE ButterflyCaseStudy.hpp)

target_link_libraries(ButterflyCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(ButterflyCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_butterfly
        FROM ButterflyCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyButterflyCaseStudy
        FROM ButterflyCaseStudy
        AS elastica::python_object
        )

elastica_python_add_dependencies(PyButterflyCaseStudy PyCosseratRods)
