add_executable(butterfly_ablation_case_options ablation.cpp)

if (NOT TARGET cxxopts::cxxopts)
    include(SetupCxxOpts)
endif ()

target_link_libraries(butterfly_ablation_case_options
        PRIVATE
        ButterflyCaseStudy
        Utilities
        CaseStudyOptionsParser
        # Needed for taking in the input file
        cxxopts::cxxopts
        )
target_include_directories(butterfly_ablation_case_options PRIVATE ${CMAKE_SOURCE_DIR})
