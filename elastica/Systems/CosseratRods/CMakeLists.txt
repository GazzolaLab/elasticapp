add_subdirectory(Components)

set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        CosseratRodPlugin.hpp
        CosseratRodTraits.hpp
)
