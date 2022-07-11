# Distributed under the MIT License. See LICENSE.txt for details.

add_custom_target(libs)

function(add_elastica_library LIBRARY_NAME)
    add_library(${LIBRARY_NAME} ${ARGN})
    add_dependencies(libs ${LIBRARY_NAME})

    get_target_property(
            LIBRARY_TYPE
            ${LIBRARY_NAME}
            TYPE
    )

    if (${LIBRARY_TYPE} STREQUAL INTERFACE_LIBRARY)
        target_link_libraries(
                ${LIBRARY_NAME}
                INTERFACE
                ElasticaFlags
        )
    else ()
        target_link_libraries(
                ${LIBRARY_NAME}
                PUBLIC
                ElasticaFlags
        )
        set_property(
                TARGET ${LIBRARY_NAME}
                PROPERTY FOLDER ${CMAKE_CURRENT_SOURCE_DIR}
        )
    endif ()
    if (BUILD_SHARED_LIBS
            AND NOT ${LIBRARY_TYPE} STREQUAL INTERFACE_LIBRARY)
        install(TARGETS ${LIBRARY_NAME} OPTIONAL
                LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR})
    endif ()
endfunction()
