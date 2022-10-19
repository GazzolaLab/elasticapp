add_library(ParallelUpdateDynamicsCaseStudy STATIC ParallelUpdateDynamicsCaseStudy.cpp)
target_sources(ParallelUpdateDynamicsCaseStudy INTERFACE ParallelUpdateDynamicsCaseStudy.hpp)

target_link_libraries(ParallelUpdateDynamicsCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        Parallel
        ElasticaFlags
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(ParallelUpdateDynamicsCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})

add_executable(update_dynamics_case_options main.cpp)
target_sources(update_dynamics_case_options INTERFACE ParallelUpdateDynamicsCaseStudy.hpp)

if (NOT TARGET cxxopts::cxxopts)
    include(SetupCxxOpts)
endif ()

target_link_libraries(update_dynamics_case_options
        PRIVATE
        ParallelUpdateDynamicsCaseStudy
        Utilities
        CaseStudyOptionsParser
        Parallel
        # Needed for taking in the input file
        cxxopts::cxxopts
        )
target_include_directories(update_dynamics_case_options PRIVATE ${CMAKE_SOURCE_DIR})
