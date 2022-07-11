set(LIBRARY ContactCacheContainerPolicy)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        NonSequentialPolicy.hpp
        SequentialPolicy.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        NonSequentialPolicy.hpp
        SequentialPolicy.hpp
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Parallel
        Utilities
)
