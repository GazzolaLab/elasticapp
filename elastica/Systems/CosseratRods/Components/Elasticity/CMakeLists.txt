add_subdirectory(detail)

set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Components.hpp
        ExplicitDampingAdapter.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        WithDiagonalLinearHyperElasticModel.hpp
        WithExplicitlyDampedDiagonalLinearHyperElasticModel.hpp
)
