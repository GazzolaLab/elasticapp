add_elastica_example(example_simulation_reset simulation_reset.cpp)

target_link_libraries(example_simulation_reset
        PUBLIC
        Simulator
        Systems
        Utilities
        Materials
        ElasticaFlags
        )

