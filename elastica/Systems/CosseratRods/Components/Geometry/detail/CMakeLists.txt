set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Components.hpp
        CosseratRodCrossSectionInterface.hpp
        Tags/CosseratRodCrossSectionInterfaceTags.hpp
        CosseratRodSpanwiseGeometry.hpp
        Tags/CosseratRodSpanwiseGeometryTags.hpp
        RodSpanwiseGeometry.hpp
        Tags/RodSpanwiseGeometryTags.hpp
        CircleCrossSectionOperations.hpp
        SquareCrossSectionOperations.hpp
        Tags.hpp
        Types.hpp
)
