# Distributed under the MIT License. See LICENSE for details.

set(executable RunTests)

# set(ELASTICA_TESTS "RunTests.cpp")

set(ELASTICA_COMPILATION_TESTS "TestCompilationFramework.cpp")

set_property(GLOBAL PROPERTY ELASTICA_TESTS_LIB_FUNCTIONS_PROPERTY "")
set_property(GLOBAL PROPERTY ELASTICA_TESTS_LIBS_PROPERTY "")

add_subdirectory(Utilities)

write_test_registration_function(
    "${CMAKE_BINARY_DIR}/tests/Unit/RunTestsRegister.hpp"
    "register_run_tests_libs")

get_property(ELASTICA_TESTS_LIBRARIES GLOBAL
             PROPERTY ELASTICA_TESTS_LIBS_PROPERTY)

add_executable(${executable} ${executable}.cpp)

elastica_add_catch_tests(${executable})

target_link_libraries(${executable} ${ELASTICA_TESTS_LIBRARIES}
                      ${ELASTICA_LIBRARIES})

# Setup code coverage for unit tests
if(COVERAGE)
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
endif()

# Setup tests of static_assert's and other compilation failures

set(EXECUTABLE compilation-failure-tests)

add_executable(${EXECUTABLE} ${ELASTICA_COMPILATION_TESTS})

set_target_properties(
    ${EXECUTABLE}
    PROPERTIES
        COMPILE_FLAGS
        "\${WHICH_TEST} -DFILE_IS_COMPILATION_TEST=\"static_assert(false, \\\"File is compilation test\\\");\""
        EXCLUDE_FROM_ALL
        TRUE)

add_compilation_tests(${EXECUTABLE})

# add_subdirectory(RunSingleTest)

# target_include_directories( ${executable} SYSTEM PUBLIC ${NUMPY_INCLUDE_DIRS}
# PUBLIC ${PYTHON_INCLUDE_DIRS} )

# target_link_libraries(${executable} Catch)
# target_include_directories(${executable})
