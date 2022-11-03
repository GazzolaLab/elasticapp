add_library(NestCaseWithWallStudy STATIC NestCaseWithWallStudy.cpp)
target_sources(NestCaseWithWallStudy INTERFACE NestCaseWithWallStudy.hpp)

target_link_libraries(NestCaseWithWallStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        Parallel
        CollisionSystem
        IO
        )


include(ElasticaCaseStudyHelpers)
add_elastica_case_study(example_nest_with_wall
        FROM NestCaseWithWallStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyNestCaseWithWallStudy
        FROM NestCaseWithWallStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyNestCaseWithWallStudy PRIVATE Blaze Parallel)
elastica_python_add_dependencies(PyNestCaseWithWallStudy PyArrays)


add_executable(example_nest_with_wall_scaling main.cpp)
target_sources(example_nest_with_wall_scaling INTERFACE NestCaseWithWallStudy.hpp)
if (NOT TARGET cxxopts::cxxopts)
    include(SetupCxxOpts)
endif ()

target_link_libraries(example_nest_with_wall_scaling
        PRIVATE
	NestCaseWithWallStudy
        Utilities
        # Needed for taking in the input file
        cxxopts::cxxopts
        )

target_include_directories(example_nest_with_wall_scaling PRIVATE ${CMAKE_SOURCE_DIR})
