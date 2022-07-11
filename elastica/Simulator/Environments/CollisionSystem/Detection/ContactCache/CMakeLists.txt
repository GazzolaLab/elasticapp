set(LIBRARY ContactCache)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(ContainerPolicy)
add_library(${LIBRARY}::ContainerPolicy ALIAS ContactCacheContainerPolicy)


elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ContactCache.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ContactCache.hpp
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ContactCacheContainerPolicy
        ErrorHandling
        Utilities
)
