add_library(FlexibleSwingingPendulumCaseStudy STATIC FlexibleSwingingPendulumCaseStudy.cpp)
target_sources(FlexibleSwingingPendulumCaseStudy INTERFACE FlexibleSwingingPendulumCaseStudy.hpp)

target_link_libraries(FlexibleSwingingPendulumCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(FlexibleSwingingPendulumCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_flexible_swinging_pendulum
        FROM FlexibleSwingingPendulumCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyFlexibleSwingingPendulumCaseStudy
        FROM FlexibleSwingingPendulumCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyFlexibleSwingingPendulumCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyFlexibleSwingingPendulumCaseStudy PyArrays)
