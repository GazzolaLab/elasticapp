add_subdirectory(Components)
add_subdirectory(Utility)
add_subdirectory(Initializers)

set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        CosseratRodPlugin.hpp
        CosseratRodTraits.hpp
)
