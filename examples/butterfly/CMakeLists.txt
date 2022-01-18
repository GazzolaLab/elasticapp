add_executable(example_butterfly butterfly.cpp)

target_link_libraries(example_butterfly
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

