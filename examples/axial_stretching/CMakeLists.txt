add_library(AxialStretchingCaseStudy STATIC AxialStretchingCaseStudy.cpp)
target_sources(AxialStretchingCaseStudy INTERFACE AxialStretchingCaseStudy.hpp)

target_link_libraries(AxialStretchingCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        )

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_axial_stretching
        FROM AxialStretchingCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyAxialStretchingCaseStudy
        FROM AxialStretchingCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyAxialStretchingCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyAxialStretchingCaseStudy PyBlaze)
