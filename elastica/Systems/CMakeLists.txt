set(LIBRARY Systems)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Block)
add_subdirectory(common)
add_subdirectory(RigidBody)
add_subdirectory(CosseratRods)
add_subdirectory(States)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Systems.hpp
        Tags.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Blaze
        ErrorHandling
        Utilities
        Simulator
)
