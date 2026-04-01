add_library(FreeNestCaseStudy STATIC FreeNestCaseStudy.cpp)
target_sources(FreeNestCaseStudy INTERFACE FreeNestCaseStudy.hpp)

target_link_libraries(FreeNestCaseStudy
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

add_elastica_case_study(example_free_nest
        FROM FreeNestCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyFreeNestCaseStudy
        FROM FreeNestCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyFreeNestCaseStudy PRIVATE Blaze Parallel)
elastica_python_add_dependencies(PyFreeNestCaseStudy PyArrays)
