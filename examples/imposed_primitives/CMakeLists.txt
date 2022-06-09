add_library(ImposedPrimitiveCaseStudy STATIC ImposedPrimitiveCaseStudy.cpp)
target_sources(ImposedPrimitiveCaseStudy INTERFACE ImposedPrimitiveCaseStudy.hpp)

target_link_libraries(ImposedPrimitiveCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        CollisionSystem
        )

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_imposed_primitive
        FROM ImposedPrimitiveCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyImposedPrimitiveCaseStudy
        FROM ImposedPrimitiveCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyImposedPrimitiveCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyImposedPrimitiveCaseStudy PyArrays)
