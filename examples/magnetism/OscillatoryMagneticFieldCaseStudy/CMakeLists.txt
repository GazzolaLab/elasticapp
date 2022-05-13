add_library(OscillatoryMagneticFieldCaseStudy STATIC OscillatoryMagneticFieldCaseStudy.cpp)
target_sources(OscillatoryMagneticFieldCaseStudy INTERFACE OscillatoryMagneticFieldCaseStudy.hpp)

target_link_libraries(OscillatoryMagneticFieldCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        MagnetismExtension
        )
# elastica
target_include_directories(OscillatoryMagneticFieldCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})
# Extension
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(OscillatoryMagneticFieldCaseStudy PUBLIC ${PARENT_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_oscillatory_magnetic_field
        FROM OscillatoryMagneticFieldCaseStudy
        AS elastica::standalone_executable
        )
target_include_directories(example_oscillatory_magnetic_field PUBLIC ${PARENT_DIR})

add_elastica_case_study(PyOscillatoryMagneticFieldCaseStudy
        FROM OscillatoryMagneticFieldCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyOscillatoryMagneticFieldCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyOscillatoryMagneticFieldCaseStudy PyBlaze)
