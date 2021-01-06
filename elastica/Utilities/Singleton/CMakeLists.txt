set(LIBRARY Utilities)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Dependency.hpp
        Singleton.hpp
        Types.hpp
        TypeTraits.hpp
)
