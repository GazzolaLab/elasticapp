set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        ComposeInitializers.hpp
        Initializers.hpp
        StraightCosseratRod.hpp
)
