# Distributed under the MIT License. See LICENSE.txt for details.

add_custom_target(libs)

function(add_elastica_library LIBRARY_NAME)
    add_library(${LIBRARY_NAME} ${ARGN})
    add_dependencies(libs ${LIBRARY_NAME})
endfunction()
