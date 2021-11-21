# Distributed under the MIT License. See LICENSE for details.
add_executable(parallelism_example main.cpp)

target_include_directories(parallelism_example
        PRIVATE
        ${Boost_INCLUDE_DIRS}
        )

target_link_libraries(parallelism_example
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

