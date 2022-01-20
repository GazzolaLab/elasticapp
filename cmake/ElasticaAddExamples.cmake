# Distributed under the MIT License. See LICENSE.txt for details.

add_custom_target(all-examples)

function(add_elastica_example EXAMPLE_NAME)
    add_executable(${EXAMPLE_NAME} ${ARGN})
    add_dependencies(all-examples ${EXAMPLE_NAME})
endfunction()
