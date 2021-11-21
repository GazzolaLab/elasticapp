add_executable(example_display_variable display_variable_example.cpp)

target_link_libraries(example_display_variable
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        )


