# case-studies
add_subdirectory(axial_stretching)
add_subdirectory(butterfly)
add_subdirectory(timoshenko)
add_subdirectory(helical_buckling)

# examples
include(ElasticaAddExamples)
add_subdirectory(parallelism)
add_subdirectory(display_variables)

# tutorials
include(ElasticaAddTutorials)
add_subdirectory(tutorial_single_rod)
add_subdirectory(tutorial_rod_construction)
add_subdirectory(tutorial_blocking)
add_subdirectory(tutorial_casestudy_setup)
