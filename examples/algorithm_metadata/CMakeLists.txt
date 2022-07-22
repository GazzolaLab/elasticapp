add_elastica_example(example_algorithm_metadata main.cpp)

target_link_libraries(example_algorithm_metadata
        PUBLIC
        Simulator
        Systems
        Materials
        ElasticaFlags
        )

