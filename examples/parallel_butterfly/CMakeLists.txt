add_library(ParallelButterflyCaseStudy STATIC ParallelButterflyCaseStudy.cpp)
target_sources(ParallelButterflyCaseStudy INTERFACE ParallelButterflyCaseStudy.hpp)

target_link_libraries(ParallelButterflyCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        Parallel
        ElasticaFlags
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(ParallelButterflyCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})

add_executable(parallel_butterfly_case_options main.cpp)
target_sources(parallel_butterfly_case_options INTERFACE ParallelButterflyCaseStudy.hpp)

if (NOT TARGET cxxopts::cxxopts)
    include(SetupCxxOpts)
endif ()

target_link_libraries(parallel_butterfly_case_options
        PRIVATE
        ParallelButterflyCaseStudy
        Utilities
        CaseStudyOptionsParser
        Parallel
        # Needed for taking in the input file
        cxxopts::cxxopts
        )
target_include_directories(parallel_butterfly_case_options PRIVATE ${CMAKE_SOURCE_DIR})

add_subdirectory(nosync)
##

#configure_file(
#        "${CMAKE_CURRENT_SOURCE_DIR}/input_file.yml"
#        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/input_file.yml"
#        COPYONLY)
