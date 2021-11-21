add_executable(Example_single_rod main.cpp)

target_link_libraries(Example_single_rod
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

