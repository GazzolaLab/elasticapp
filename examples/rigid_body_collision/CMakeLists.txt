add_library(RigidBodyCollisionCaseStudy STATIC RigidBodyCollisionCaseStudy.cpp)
target_sources(RigidBodyCollisionCaseStudy INTERFACE RigidBodyCollisionCaseStudy.hpp)

target_link_libraries(RigidBodyCollisionCaseStudy
        PRIVATE
        Utilities
        Simulator
        CollisionSystem
        Systems
        Materials
        ElasticaFlags
        )
# So that I can code up as <elastica/CaseStudy> rather than "CaseStudy"
target_include_directories(RigidBodyCollisionCaseStudy PRIVATE ${CMAKE_SOURCE_DIR})

include(ElasticaCaseStudyHelpers)

add_elastica_case_study(example_rigid_body_collision
        FROM RigidBodyCollisionCaseStudy
        AS elastica::standalone_executable
        )

add_elastica_case_study(PyRigidBodyCollisionCaseStudy
        FROM RigidBodyCollisionCaseStudy
        AS elastica::python_object
        )
#
elastica_python_link_libraries(PyRigidBodyCollisionCaseStudy PRIVATE Blaze)
elastica_python_add_dependencies(PyRigidBodyCollisionCaseStudy PyArrays)
