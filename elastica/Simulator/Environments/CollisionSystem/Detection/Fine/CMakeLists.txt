set(LIBRARY FineCollisionDetection)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ContactTrait.hpp
        Detectors.hpp
        EPA.hpp
        FineDetectionInterface.hpp
        GJK.hpp
        Kernels.hpp
        MaxContacts.hpp
        RigidBodyPrimitiveTrait.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ContactTrait.hpp
        Detectors.hpp
        EPA.hpp
        FineDetectionInterface.hpp
        GJK.hpp
        Kernels.hpp
        MaxContacts.hpp
        RigidBodyPrimitiveTrait.hpp
        Types.hpp
)

add_subdirectory(Kernels)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Contact
        Primitives
        ErrorHandling
        Utilities
        FineCollisionDetectionKernels
)
