add_elastica_example(Example_display_variable main.cpp)

target_link_libraries(Example_display_variable
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        )


