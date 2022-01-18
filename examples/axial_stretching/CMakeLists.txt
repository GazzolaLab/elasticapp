add_executable(axial_stretching axial_stretching.cpp)

target_link_libraries(axial_stretching
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        Simulator
        Materials
        )

