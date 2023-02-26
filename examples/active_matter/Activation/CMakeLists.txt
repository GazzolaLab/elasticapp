add_subdirectory(Funcs)

set(LIBRARY Activation)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Funcs.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)

target_link_libraries(${LIBRARY}
        INTERFACE
        ActivationFuncs
        )
