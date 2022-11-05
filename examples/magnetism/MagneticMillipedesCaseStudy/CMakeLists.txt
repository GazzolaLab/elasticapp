add_library(MagneticMillipedesCaseStudy STATIC MagneticMillipedesCaseStudy.cpp)
target_sources(MagneticMillipedesCaseStudy INTERFACE MagneticMillipedesCaseStudy.hpp CustomConnection.cpp)

target_link_libraries(MagneticMillipedesCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        MagnetismExtension
        )
# elastica
target_include_directories(MagneticMillipedesCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})
# Extension
get_filename_component(PARENT_DIR ${CMAKE_CURRENT_SOURCE_DIR} DIRECTORY)
target_include_directories(MagneticMillipedesCaseStudy PUBLIC ${PARENT_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_magnetic_millipedes
        FROM MagneticMillipedesCaseStudy
        AS elastica::standalone_executable
        )
target_include_directories(example_magnetic_millipedes PUBLIC ${PARENT_DIR})


# Add python bindings to rods first
set(LIBRARY "PyMagneticMillipedeCosseratRods")

elastica_python_add_module(
        MagneticMillipedeCosseratRods
        MODULE_PATH "Systems/"
        LIBRARY_NAME ${LIBRARY}
        SOURCES
        Bindings.cpp
)

elastica_python_link_libraries(
        ${LIBRARY}
        PRIVATE
        CosseratRods
        pybind11::module
        PyGeneratorsCosseratRod
        Blaze
)

elastica_python_add_dependencies(${LIBRARY} PyArrays PyTags)

add_elastica_case_study(PyMagneticMillipedesCaseStudy
        FROM MagneticMillipedesCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyMagneticMillipedesCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyMagneticMillipedesCaseStudy ${LIBRARY} PyMagnetizedCosseratRods)
