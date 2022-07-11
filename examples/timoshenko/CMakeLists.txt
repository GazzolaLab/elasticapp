add_library(TimoshenkoCaseStudy STATIC TimoshenkoCaseStudy.cpp)
target_sources(TimoshenkoCaseStudy INTERFACE TimoshenkoCaseStudy.hpp)

target_link_libraries(TimoshenkoCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(TimoshenkoCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_timoshenko
        FROM TimoshenkoCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyTimoshenkoCaseStudy
        FROM TimoshenkoCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyTimoshenkoCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyTimoshenkoCaseStudy PyArrays)
