add_library(DynamicCantileverCaseStudy STATIC DynamicCantileverCaseStudy.cpp)
target_sources(DynamicCantileverCaseStudy INTERFACE DynamicCantileverCaseStudy.hpp)

target_link_libraries(DynamicCantileverCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(DynamicCantileverCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(PyDynamicCantileverCaseStudy
        FROM DynamicCantileverCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyDynamicCantileverCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyDynamicCantileverCaseStudy PyCosseratRods)
