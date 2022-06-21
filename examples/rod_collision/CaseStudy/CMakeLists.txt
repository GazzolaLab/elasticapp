add_library(RodCollisionCaseStudy STATIC RodCollisionCaseStudy.cpp)
target_sources(RodCollisionCaseStudy INTERFACE RodCollisionCaseStudy.hpp)

target_link_libraries(RodCollisionCaseStudy
        PRIVATE
        Utilities
        Simulator
        CollisionSystem
        Systems
        Materials
        ElasticaFlags
        )

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_rod_collision
        FROM RodCollisionCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyRodCollisionCaseStudy
        FROM RodCollisionCaseStudy
        AS elastica::python_object
        )
#
#elastica_python_link_libraries(PyRodCollisionCaseStudy PRIVATE PyCosseratRods)
elastica_python_add_dependencies(PyRodCollisionCaseStudy PyCosseratRods)
