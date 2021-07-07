set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Components.hpp
        ElasticityInterface.hpp
        ElasticityInterfaceTags.hpp
        ExplicitDampingAdapter.hpp
        ExplicitDampingAdapterFacade.hpp
        ExplicitDampingAdapterFacadeTags.hpp
        LinearHyperElasticityFacade.hpp
        LinearHyperElasticityFacadeTags.hpp
        Tags.hpp
        Types.hpp
        TypeTraits.hpp
)
