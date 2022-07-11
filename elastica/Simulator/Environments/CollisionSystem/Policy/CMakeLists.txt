set(LIBRARY CollisionPolicy)

add_library(${LIBRARY} INTERFACE)

add_subdirectory(ExecutionPolicy)
add_library(${LIBRARY}::ExecutionPolicy ALIAS CollisionExecutionPolicy)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        # DomainPolicy.hpp
        ExecutionPolicy.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        # DomainPolicy.hpp
        ExecutionPolicy.hpp
)
