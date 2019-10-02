# Distributed under the MIT License. See LICENSE for details.

add_executable(elastica main.cpp)

target_include_directories(elastica PRIVATE DataStructures)
target_include_directories(elastica PRIVATE Utilities)

add_subdirectory(DataStructures)

target_link_libraries(elastica PUBLIC ${ELASTICA_LIBRARIES})
