# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodComponents)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Components.hpp
        ElasticityInterface.hpp
        Tags/ElasticityInterfaceTags.hpp
        ExplicitDampingAdapter.hpp
        ExplicitDampingAdapterFacade.hpp
        Tags/ExplicitDampingAdapterFacadeTags.hpp
        LinearHyperElasticityFacade.hpp
        Tags/LinearHyperElasticityFacadeTags.hpp
        Tags.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Components.hpp
        ElasticityInterface.hpp
        Tags/ElasticityInterfaceTags.hpp
        ExplicitDampingAdapter.hpp
        ExplicitDampingAdapterFacade.hpp
        Tags/ExplicitDampingAdapterFacadeTags.hpp
        LinearHyperElasticityFacade.hpp
        Tags/LinearHyperElasticityFacadeTags.hpp
        Tags.hpp
        Types.hpp
        TypeTraits.hpp
)
