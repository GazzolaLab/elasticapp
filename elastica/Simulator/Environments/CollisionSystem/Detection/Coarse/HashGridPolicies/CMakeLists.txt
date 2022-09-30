set(LIBRARY HashGridPolicies)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        DomainPolicy.hpp
        HashGridsSpanPolicy.hpp
        StoragePolicy.hpp
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

add_subdirectory(DomainPolicies)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        HashGridDomainPolicies
        Stl
)
