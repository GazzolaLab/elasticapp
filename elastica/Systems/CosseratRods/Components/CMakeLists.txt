# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodComponents)
add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Elasticity.hpp
        Geometry.hpp
        helpers.hpp
        Initialization.hpp
        Kinematics.hpp
        Names.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)

add_subdirectory(helpers)
add_subdirectory(Geometry)
add_subdirectory(Elasticity)
add_subdirectory(Kinematics)
add_subdirectory(Names)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ErrorHandling
        Utilities
        SystemsCommon
        Frames # For cast_along
)
