set(LIBRARY Utilities)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Affordances.hpp
        CRTP.hpp
        DefaultTag.hpp
        NamedType.hpp
        Traits.hpp
        Types.hpp
)
