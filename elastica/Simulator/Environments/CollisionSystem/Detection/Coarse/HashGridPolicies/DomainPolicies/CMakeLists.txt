set(LIBRARY HashGridDomainPolicies)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        DomainPolicyBase.hpp
        PeriodicDomainPolicy.hpp
        Types.hpp
        UnboundedDomainPolicy.hpp
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

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Domain
        Utilities
        Stl
)
