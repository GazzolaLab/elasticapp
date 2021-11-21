add_executable(example_block_initialization main.cpp)

target_link_libraries(example_block_initialization
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        )

