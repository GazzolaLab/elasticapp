add_executable(zeros zeros.cpp)

target_link_libraries(zeros
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

