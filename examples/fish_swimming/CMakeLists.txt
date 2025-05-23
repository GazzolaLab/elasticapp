add_subdirectory(FishModel)

set(LIBRARY FishCaseStudy)

add_library(${LIBRARY} STATIC ${LIBRARY}.cpp)
target_sources(${LIBRARY} INTERFACE 
        ${LIBRARY}.hpp 
        )

target_link_libraries(${LIBRARY}
        PRIVATE
        Block
        SystemsCommon
        CosseratRods
        SystemsCustomization
        RigidBody
        States
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        FishModel
        )

# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(${LIBRARY} PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_elastic_fish
        FROM ${LIBRARY}
        AS elastica::standalone_executable
        )

add_elastica_case_study(Py${LIBRARY}
        FROM ${LIBRARY}
        AS elastica::python_object
        )
#
elastica_python_link_libraries(Py${LIBRARY} PRIVATE Blaze)
elastica_python_add_dependencies(Py${LIBRARY} PyArrays PyCosseratRods PyFishModel)
