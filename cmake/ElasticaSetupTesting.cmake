# Distributed under the MIT License.
# See LICENSE.txt for details.

include(CTest)

set(ELASTICA_TEST_RUNNER "" CACHE STRING
        "Run test executables through the given wrapper.")

option(ELASTICA_TEST_TIMEOUT_FACTOR
        "Multiply timeout for tests by this factor")

function(elastica_define_test_timeout_factor_option TEST_TYPE HELP_NAME)
    option(ELASTICA_${TEST_TYPE}_TEST_TIMEOUT_FACTOR
            "Multiply timeout for ${HELP_NAME} tests by this factor")
endfunction()

# Multiply BASE_TIMEOUT by the user specified option for TEST_TYPE
function(elastica_test_timeout RETURN_VARIABLE TEST_TYPE BASE_TIMEOUT)
    if (ELASTICA_${TEST_TYPE}_TEST_TIMEOUT_FACTOR)
        set(FACTOR ${ELASTICA_${TEST_TYPE}_TEST_TIMEOUT_FACTOR})
    elseif (ELASTICA_TEST_TIMEOUT_FACTOR)
        set(FACTOR ${ELASTICA_TEST_TIMEOUT_FACTOR})
    else ()
        set(FACTOR 1)
    endif ()

    # Note: "1" is parsed as "ON" by cmake
    if (NOT "${FACTOR}" STREQUAL ON)
        math(EXPR RESULT "${FACTOR} * ${BASE_TIMEOUT}")
    else ()
        set(RESULT ${BASE_TIMEOUT})
    endif ()

    set(${RETURN_VARIABLE} ${RESULT} PARENT_SCOPE)
endfunction()
