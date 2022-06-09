add_library(HingeJointCaseStudy STATIC HingeJointCaseStudy.cpp)
target_sources(HingeJointCaseStudy INTERFACE HingeJointCaseStudy.hpp)

target_link_libraries(HingeJointCaseStudy
        PRIVATE
        Utilities
        Simulator
        Systems
        Materials
        ElasticaFlags
        )

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_hinge_joint
        FROM HingeJointCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyHingeJointCaseStudy
        FROM HingeJointCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyHingeJointCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyHingeJointCaseStudy PyArrays)
