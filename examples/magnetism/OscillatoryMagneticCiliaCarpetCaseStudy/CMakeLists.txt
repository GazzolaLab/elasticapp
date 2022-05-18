add_library(OscillatoryMagneticCiliaCarpetCaseStudy STATIC OscillatoryMagneticCiliaCarpetCaseStudy.cpp)
target_sources(OscillatoryMagneticCiliaCarpetCaseStudy INTERFACE OscillatoryMagneticCiliaCarpetCaseStudy.hpp)

target_link_libraries(OscillatoryMagneticCiliaCarpetCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        MagnetismExtension
        )
# elastica
target_include_directories(OscillatoryMagneticCiliaCarpetCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})
# Extension
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(OscillatoryMagneticCiliaCarpetCaseStudy PUBLIC ${PARENT_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_oscillatory_magnetic_cilia_carpet
        FROM OscillatoryMagneticCiliaCarpetCaseStudy
        AS elastica::standalone_executable
        )
target_include_directories(example_oscillatory_magnetic_cilia_carpet PUBLIC ${PARENT_DIR})

add_elastica_case_study(PyOscillatoryMagneticCiliaCarpetCaseStudy
        FROM OscillatoryMagneticCiliaCarpetCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyOscillatoryMagneticCiliaCarpetCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyOscillatoryMagneticCiliaCarpetCaseStudy PyBlaze)
