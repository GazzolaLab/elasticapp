elastica_target_headers(
        Systems
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        States.hpp
        Types.hpp
        TypeTraits.hpp
)

add_subdirectory(Expressions)
add_subdirectory(TypeTraits)