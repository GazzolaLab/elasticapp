add_executable(parallel_butterfly_no_sync_case_options main.cpp)

if (NOT TARGET cxxopts::cxxopts)
    include(SetupCxxOpts)
endif ()

target_link_libraries(parallel_butterfly_no_sync_case_options
        PRIVATE
        ParallelButterflyCaseStudy
        Utilities
        CaseStudyOptionsParser
        Parallel
        # Needed for taking in the input file
        cxxopts::cxxopts
        )
target_include_directories(parallel_butterfly_no_sync_case_options PRIVATE ${CMAKE_SOURCE_DIR})

##

#configure_file(
#        "${CMAKE_CURRENT_SOURCE_DIR}/input_file.yml"
#        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/input_file.yml"
#        COPYONLY)
