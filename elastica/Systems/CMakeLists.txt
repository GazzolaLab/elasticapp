set(LIBRARY Systems)

add_elastica_library(${LIBRARY})

add_subdirectory(RigidBody)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Systems.hpp
        Tags.hpp
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Blaze
        ErrorHandling
        Utilities
        Simulator
)
