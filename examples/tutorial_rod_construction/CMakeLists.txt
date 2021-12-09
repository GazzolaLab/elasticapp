add_executable(tutorial_rod_construction tutorial_rod_construction.cpp)

target_link_libraries(tutorial_rod_construction
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

