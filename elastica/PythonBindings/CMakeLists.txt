if (${ELASTICA_BUILD_PYTHON_BINDINGS})
    set(LIBRARY "PythonBindings")

    add_elastica_library(${LIBRARY} INTERFACE)

    elastica_target_headers(
            ${LIBRARY}
            INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
            HEADERS
            BoundChecks.hpp
    )

    target_link_libraries(${LIBRARY}
            INTERFACE
            Utilities)
endif ()
