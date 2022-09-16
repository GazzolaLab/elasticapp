# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY H5Systems)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        BlockSlice.hpp
        CosseratRods.hpp
        RigidBody.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)

add_subdirectory(CosseratRods)
add_library(${LIBRARY}::CosseratRods ALIAS H5CosseratRods)

add_subdirectory(RigidBody)
add_library(${LIBRARY}::RigidBody ALIAS H5RigidBody)


target_link_libraries(
        ${LIBRARY}
        INTERFACE
        H5CosseratRods
        H5RigidBody
)
