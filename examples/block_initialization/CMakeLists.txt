# Distributed under the MIT License. See LICENSE for details.
add_executable(block_initialization main.cpp)

##add_subdirectory(DataStructures)
#add_subdirectory(ErrorHandling)
#add_subdirectory(Utilities)
#add_subdirectory(Simulator)
#add_subdirectory(Systems)
#add_subdirectory(Time)

target_link_libraries(block_initialization
        PUBLIC
        Utilities
        ErrorHandling
        Blaze
        Systems
        ElasticaFlags
        )

