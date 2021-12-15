add_elastica_tutorial(tutorial_single_rod tutorial_single_rod.cpp)

target_link_libraries(tutorial_single_rod
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

