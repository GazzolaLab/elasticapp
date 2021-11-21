add_executable(Example_block_initialization main.cpp)

target_link_libraries(Example_block_initialization
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        )

