# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodComponents)
add_subdirectory(detail)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Components.hpp
        ExplicitDampingAdapter.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        WithDiagonalLinearHyperElasticModel.hpp
        WithExplicitlyDampedDiagonalLinearHyperElasticModel.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Components.hpp
        ExplicitDampingAdapter.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        WithDiagonalLinearHyperElasticModel.hpp
        WithExplicitlyDampedDiagonalLinearHyperElasticModel.hpp
)
