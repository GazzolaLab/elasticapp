add_elastica_example(example_print_algorithm main.cpp)

target_link_libraries(example_print_algorithm
        PUBLIC
        Utilities
        ErrorHandling
        Simulator
        Systems
        Utilities
        Materials
        ElasticaFlags
        )

