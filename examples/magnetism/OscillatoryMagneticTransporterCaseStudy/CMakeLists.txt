add_library(OscillatoryMagneticTransporterCaseStudy STATIC OscillatoryMagneticTransporterCaseStudy.cpp)
target_sources(OscillatoryMagneticTransporterCaseStudy INTERFACE OscillatoryMagneticTransporterCaseStudy.hpp)

target_link_libraries(OscillatoryMagneticTransporterCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        MagnetismExtension
        )
# elastica
target_include_directories(OscillatoryMagneticTransporterCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})
# Extension
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(OscillatoryMagneticTransporterCaseStudy PUBLIC ${PARENT_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_oscillatory_magnetic_transporter
        FROM OscillatoryMagneticTransporterCaseStudy
        AS elastica::standalone_executable
        )
target_include_directories(example_oscillatory_magnetic_transporter PUBLIC ${PARENT_DIR})

add_elastica_case_study(PyOscillatoryMagneticTransporterCaseStudy
        FROM OscillatoryMagneticTransporterCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyOscillatoryMagneticTransporterCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyOscillatoryMagneticTransporterCaseStudy PyCosseratRods PyMagnetizedCosseratRods PyRigidBody)
