# Distributed under the MIT License. See LICENSE.txt for details.

add_custom_target(all-tutorials)

function(add_elastica_tutorial TUTORIAL_NAME)
    add_executable(${TUTORIAL_NAME} ${ARGN})
    add_dependencies(all-tutorials ${TUTORIAL_NAME})
endfunction()
