add_elastica_example(example_parallelism main.cpp)

target_link_libraries(example_parallelism
        PUBLIC
        Utilities
        ErrorHandling
        Simulator
        Systems
        Utilities
        Materials
        # link against TBB as well
        TBB
        ElasticaFlags
        )

