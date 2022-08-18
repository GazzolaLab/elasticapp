add_library(RodPlaneCollisionCaseStudy STATIC RodPlaneCollisionCaseStudy.cpp)
target_sources(RodPlaneCollisionCaseStudy INTERFACE RodPlaneCollisionCaseStudy.hpp)

target_link_libraries(RodPlaneCollisionCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        CollisionSystem
        )

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_rod_plane_collision
        FROM RodPlaneCollisionCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyRodPlaneCollisionCaseStudy
        FROM RodPlaneCollisionCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyRodPlaneCollisionCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyRodPlaneCollisionCaseStudy PyCosseratRods)

