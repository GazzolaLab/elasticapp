add_executable(Example_parallelism main.cpp)

target_include_directories(Example_parallelism
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        )

target_link_libraries(Example_parallelism
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

