add_subdirectory(block_initialization)

# Distributed under the MIT License. See LICENSE for details.
add_executable(DisplayVariableExample display_variable_example.cpp)

target_link_libraries(DisplayVariableExample
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        )


add_executable(SingleRodExample single_rod.cpp)
target_link_libraries(SingleRodExample
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )
