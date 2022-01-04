add_executable(timoshenko timoshenko.cpp)

target_link_libraries(timoshenko
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

