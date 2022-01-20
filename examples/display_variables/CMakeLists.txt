add_elastica_example(example_display_variable main.cpp)

target_link_libraries(example_display_variable
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        )


