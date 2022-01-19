add_elastica_tutorial(tutorial_blocking tutorial_blocking.cpp)

target_link_libraries(tutorial_blocking
        PUBLIC
        Utilities
        ErrorHandling
        Systems
        ElasticaFlags
        Simulator
        )

