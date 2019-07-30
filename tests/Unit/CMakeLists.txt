# Distributed under the MIT License. See LICENSE for details.

set(executable RunTests)

set(ELASTICA_TESTS "TestFramework.cpp")

add_executable(${executable} ${ELASTICA_TESTS})

elastica_add_catch_tests(${executable})

# target_link_libraries(${executable} Catch)
# target_include_directories(${executable})
