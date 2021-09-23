set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Components.hpp
        CosseratRodCrossSectionInterface.hpp
        CosseratRodCrossSectionInterfaceTags.hpp
        CosseratRodSpanwiseGeometry.hpp
        CosseratRodSpanwiseGeometryTags.hpp
        RodSpanwiseGeometry.hpp
        RodSpanwiseGeometryTags.hpp
        CircleCrossSectionOperations.hpp
        SquareCrossSectionOperations.hpp
        Tags.hpp
        Types.hpp
)
