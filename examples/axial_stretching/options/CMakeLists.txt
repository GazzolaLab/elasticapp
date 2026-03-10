add_executable(axial_stretching_case_options axial_stretching_case_options.cpp)
target_sources(axial_stretching_case_options INTERFACE AxialStretchingCaseStudy.hpp)

if (NOT TARGET cxxopts::cxxopts)
    include(SetupCxxOpts)
endif ()

target_link_libraries(axial_stretching_case_options
        PRIVATE
        AxialStretchingCaseStudy
        Utilities
        CaseStudyOptionsParser
        # Needed for taking in the input file
        cxxopts::cxxopts
        yaml-cpp::yaml-cpp
        )
##

configure_file(
        "${CMAKE_CURRENT_SOURCE_DIR}/input_file.yml"
        "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}/input_file.yml"
        COPYONLY)