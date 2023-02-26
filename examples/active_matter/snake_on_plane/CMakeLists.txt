add_library(SnakeOnPlane STATIC SnakeOnPlane.cpp)
target_sources(SnakeOnPlane INTERFACE SnakeOnPlane.hpp)

target_link_libraries(SnakeOnPlane
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        Activation
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(SnakeOnPlane PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_snake_on_plane
        FROM SnakeOnPlane
        AS elastica::standalone_executable
        )

add_elastica_case_study(PySnakeOnPlane
        FROM SnakeOnPlane
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PySnakeOnPlane PRIVATE Blaze)
elastica_python_add_dependencies(PySnakeOnPlane PyCosseratRods)
