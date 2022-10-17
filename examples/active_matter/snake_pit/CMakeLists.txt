if (TARGET H5)
    add_library(SnakePit STATIC SnakePit.cpp)
    target_sources(SnakePit INTERFACE SnakePit.hpp)

    target_link_libraries(SnakePit
            PRIVATE
            Utilities
            Simulator
            Systems
            Materials
            ElasticaFlags
            Activation
            H5
            PUBLIC
            Parallel
            )
    # So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
    target_include_directories(SnakePit PRIVATE ${CMAKE_SOURCE_DIR})

    include(ElasticaCaseStudyHelpers)

    add_elastica_case_study(example_snake_pit
            FROM SnakePit
            AS elastica::standalone_executable
            )

    add_elastica_case_study(PySnakePit
            FROM SnakePit
            AS elastica::python_object
            )
    #
    elastica_python_link_libraries(PySnakePit PRIVATE Blaze Parallel)
    elastica_python_add_dependencies(PySnakePit PyCosseratRods)
endif ()
