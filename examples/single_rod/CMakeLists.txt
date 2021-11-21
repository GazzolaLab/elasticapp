add_executable(example_single_rod single_rod.cpp)

target_link_libraries(example_single_rod
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

