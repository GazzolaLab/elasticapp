# Distributed under the MIT License. See LICENSE for details.

add_executable(elastica main.cpp)

target_link_libraries(elastica PUBLIC ${ELASTICA_LIBRARIES})
