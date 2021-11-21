add_executable(example_timestep_rod timestep_rod.cpp)

target_link_libraries(example_timestep_rod
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

