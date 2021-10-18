add_subdirectory(Components)
add_subdirectory(Utility)

set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        CosseratRodPlugin.hpp
        CosseratRodTraits.hpp
)
