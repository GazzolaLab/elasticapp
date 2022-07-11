set(LIBRARY CollisionSystem)
elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        DomainPolicy.hpp
        PeriodicDomainPolicy.hpp
        UnboundedDomainPolicy.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        DomainPolicy.hpp
        PeriodicDomainPolicy.hpp
        UnboundedDomainPolicy.hpp
)
