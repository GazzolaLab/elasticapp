add_executable(example_timoshenko timoshenko.cpp)

target_link_libraries(example_timoshenko
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

