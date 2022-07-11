# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodComponents)
add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Elasticity.hpp
        Geometry.hpp
        Kinematics.hpp
        helpers.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Elasticity.hpp
        Geometry.hpp
        Kinematics.hpp
        helpers.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
)

add_subdirectory(helpers)
add_subdirectory(Geometry)
add_subdirectory(Elasticity)
add_subdirectory(Kinematics)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ErrorHandling
        Utilities
        SystemsCommon
        Frames # For cast_along
)
