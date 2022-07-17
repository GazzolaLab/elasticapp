add_library(NestCaseWithWallStudy STATIC NestCaseWithWallStudy.cpp)
target_sources(NestCaseWithWallStudy INTERFACE NestCaseWithWallStudy.hpp)

target_link_libraries(NestCaseWithWallStudy
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

add_elastica_case_study(example_nest_with_wall
        FROM NestCaseWithWallStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyNestCaseWithWallStudy
        FROM NestCaseWithWallStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyNestCaseWithWallStudy PRIVATE Blaze Parallel)
elastica_python_add_dependencies(PyNestCaseWithWallStudy PyArrays)
