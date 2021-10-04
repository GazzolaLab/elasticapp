set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
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
