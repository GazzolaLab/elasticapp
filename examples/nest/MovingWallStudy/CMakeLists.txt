add_library(MovingWallStudy STATIC MovingWallStudy.cpp)
target_sources(MovingWallStudy INTERFACE MovingWallStudy.hpp)

target_link_libraries(MovingWallStudy
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

add_elastica_case_study(example_moving_wall
        FROM MovingWallStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyMovingWallStudy
        FROM MovingWallStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyMovingWallStudy PRIVATE Blaze Parallel)
elastica_python_add_dependencies(PyMovingWallStudy PyArrays)
