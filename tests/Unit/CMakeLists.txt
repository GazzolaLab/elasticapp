# Distributed under the MIT License. See LICENSE for details.

set(executable RunTests)

set_property(GLOBAL PROPERTY ELASTICA_TESTS_LIB_FUNCTIONS_PROPERTY "")
set_property(GLOBAL PROPERTY ELASTICA_TESTS_LIBS_PROPERTY "")

add_subdirectory(Framework)
add_subdirectory(Options)
add_subdirectory(Simulator)
add_subdirectory(Systems)
add_subdirectory(Time)
add_subdirectory(Utilities)

write_test_registration_function(
        "${CMAKE_SOURCE_DIR}/tests/Unit/RunTestsRegister.hpp"
        "register_run_tests_libs")

get_property(
        ELASTICA_TESTS_LIBRARIES
        GLOBAL
        PROPERTY
        ELASTICA_TESTS_LIBS_PROPERTY)
#message("${ELASTICA_TESTS_LIBRARIES}")
get_property(ELASTICA_TESTS_LIB_FUNCTIONS GLOBAL
        PROPERTY ELASTICA_TESTS_LIB_FUNCTIONS_PROPERTY)
#message("${ELASTICA_TESTS_LIB_FUNCTIONS}")
#message("HI")

add_executable(${executable} ${executable}.cpp)

target_link_libraries(${executable} ${ELASTICA_TESTS_LIBRARIES}
        ${ELASTICA_LIBRARIES})

add_dependencies(unit-tests ${executable})

elastica_add_catch_tests(${executable} "${ELASTICA_TESTS_LIBRARIES}")


# Setup code coverage for unit tests
if (COVERAGE)
    # Setup unit test coverage target.
    setup_target_for_coverage(Unit
            ${CMAKE_BINARY_DIR}/docs/html
            unit-test-coverage
            ${CMAKE_CTEST_COMMAND}
            DEPENDS
            ${executable}
            TESTRUNNER_ARGS
            "--output-on-failure"
            IGNORE_COV
            '${CMAKE_BINARY_DIR}/Informer/InfoAtLink.cpp'
            '${CMAKE_SOURCE_DIR}/tests/*')
endif ()

# add_subdirectory(RunSingleTest)

# target_include_directories( ${executable} SYSTEM PUBLIC ${NUMPY_INCLUDE_DIRS}
# PUBLIC ${PYTHON_INCLUDE_DIRS} )

# target_link_libraries(${executable} Catch)
# target_include_directories(${executable})
