set(LIBRARY UnionFindExecutionPolicy)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ParallelExecutionPolicy.hpp
        SequentialExecutionPolicy.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PUBLIC
        ParallelExecutionPolicy.hpp
        SequentialExecutionPolicy.hpp
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        Primitives
        Contact
        Parallel
)
