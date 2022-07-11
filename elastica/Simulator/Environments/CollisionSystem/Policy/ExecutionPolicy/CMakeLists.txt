set(LIBRARY CollisionExecutionPolicy)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        KernelExecutionPolicy.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Parallel
        Utilities
)
