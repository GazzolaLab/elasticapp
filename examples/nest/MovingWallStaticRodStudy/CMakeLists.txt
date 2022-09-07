add_library(MovingWallStaticRodStudy STATIC MovingWallStaticRodStudy.cpp)
target_sources(MovingWallStaticRodStudy INTERFACE MovingWallStaticRodStudy.hpp)

target_link_libraries(MovingWallStaticRodStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        Parallel
        CollisionSystem
        )

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_moving_wall_static_rod
        FROM MovingWallStaticRodStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyMovingWallStaticRodStudy
        FROM MovingWallStaticRodStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyMovingWallStaticRodStudy PRIVATE Blaze Parallel)
elastica_python_add_dependencies(PyMovingWallStaticRodStudy PyArrays)
