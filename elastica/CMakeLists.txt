# Distributed under the MIT License. See LICENSE for details.

add_executable(elastica main.cpp)

target_include_directories(elastica PRIVATE DataStructures)
target_include_directories(elastica PRIVATE Utilities)
target_include_directories(elastica PRIVATE Time)
target_include_directories(elastica PRIVATE Simulator)

add_subdirectory(DataStructures)
add_subdirectory(ErrorHandling)
add_subdirectory(Utilities)
add_subdirectory(Simulator)

target_link_libraries(elastica PUBLIC ${ELASTICA_LIBRARIES})
target_link_libraries(elastica PUBLIC Utilities)
