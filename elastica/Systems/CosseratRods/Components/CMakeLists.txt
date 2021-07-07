add_subdirectory(helpers)
add_subdirectory(Geometry)
add_subdirectory(Elasticity)
add_subdirectory(Kinematics)

set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Elasticity.hpp
        Geometry.hpp
        Kinematics.hpp
        Tags.hpp
        Types.hpp
)
