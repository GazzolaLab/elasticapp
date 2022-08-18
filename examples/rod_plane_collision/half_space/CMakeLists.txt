add_library(RodHalfSpaceCollisionCaseStudy STATIC RodHalfSpaceCollisionCaseStudy.cpp)
target_sources(RodHalfSpaceCollisionCaseStudy INTERFACE RodHalfSpaceCollisionCaseStudy.hpp)

target_link_libraries(RodHalfSpaceCollisionCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        CollisionSystem
        )

add_elastica_case_study(example_rod_half_space_collision
        FROM RodHalfSpaceCollisionCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyRodHalfSpaceCollisionCaseStudy
        FROM RodHalfSpaceCollisionCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyRodHalfSpaceCollisionCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyRodHalfSpaceCollisionCaseStudy PyCosseratRods)
