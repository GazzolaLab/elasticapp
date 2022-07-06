# case-studies
add_subdirectory(axial_stretching)
add_subdirectory(butterfly)
add_subdirectory(timoshenko)
add_subdirectory(helical_buckling)
add_subdirectory(flexible_swinging_pendulum)
add_subdirectory(hinge_joint)
add_subdirectory(imposed_primitives)
add_subdirectory(rigid_body_collision)
add_subdirectory(rod_collision)
add_subdirectory(rod_plane_collision)

# examples
include(ElasticaAddExamples)
add_subdirectory(parallelism)
add_subdirectory(display_variables)
add_subdirectory(simulation_reset)

# tutorials
include(ElasticaAddTutorials)
add_subdirectory(tutorial_single_rod)
add_subdirectory(tutorial_rod_construction)
add_subdirectory(tutorial_blocking)
add_subdirectory(tutorial_casestudy_setup)


# ignore
if (IS_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/local")
    add_subdirectory(local)
endif ()