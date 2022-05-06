add_library(ConstantMagneticFieldCaseStudy STATIC ConstantMagneticFieldCaseStudy.cpp)
target_sources(ConstantMagneticFieldCaseStudy INTERFACE ConstantMagneticFieldCaseStudy.hpp)

target_link_libraries(ConstantMagneticFieldCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        MagnetismExtension
        )
# elastica
target_include_directories(ConstantMagneticFieldCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})
# Extension
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(ConstantMagneticFieldCaseStudy PUBLIC ${PARENT_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_constant_magnetic_field
        FROM ConstantMagneticFieldCaseStudy
        AS elastica::standalone_executable
        )
target_include_directories(example_constant_magnetic_field PUBLIC ${PARENT_DIR})

add_elastica_case_study(PyConstantMagneticFieldCaseStudy
        FROM ConstantMagneticFieldCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyConstantMagneticFieldCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyConstantMagneticFieldCaseStudy PyBlaze)
