set(LIBRARY DEMExecutionPolicy)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ExecutionPolicy.hpp
        ParallelContactResolver.hpp
        ParallelExecutionPolicy.hpp
        SequentialContactResolver.hpp
        SequentialExecutionPolicy.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ExecutionPolicy.hpp
        ParallelContactResolver.hpp
        ParallelExecutionPolicy.hpp
        SequentialContactResolver.hpp
        SequentialExecutionPolicy.hpp
        Types.hpp
)


target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Contact
        Utilities
        Parallel
)

