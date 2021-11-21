add_executable(Example_timestep_rod main.cpp)

target_link_libraries(Example_timestep_rod
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

