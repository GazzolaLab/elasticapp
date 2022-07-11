set(LIBRARY DEMForces)

add_elastica_library(${LIBRARY})

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        LinearSpringDashpot.hpp
        Protocols.hpp
        Types.hpp
        # TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        LinearSpringDashpot.cpp
        PUBLIC
        LinearSpringDashpot.hpp
        Protocols.hpp
        Types.hpp
        # TypeTraits.hpp
)

target_link_libraries(
        ${LIBRARY}
        PRIVATE
        Contact
        PUBLIC
        Utilities
        ErrorHandling
)
