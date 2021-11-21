add_executable(example_parallelism main.cpp)

target_include_directories(example_parallelism
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        )

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

