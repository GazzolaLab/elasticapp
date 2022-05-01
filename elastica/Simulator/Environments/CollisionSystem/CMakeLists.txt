# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CollisionSystem)

add_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        CollisionSystem.hpp
        Configuration.hpp
        Tags.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        CollisionSystem.hpp
        Configuration.hpp
        Tags.hpp
        Types.hpp
)

add_subdirectory(Batches)
add_library(${LIBRARY}::CollisionBatches ALIAS CollisionBatches)

add_subdirectory(Configuration)
add_library(${LIBRARY}::Configuration ALIAS CollisionSystemConfiguration)

add_subdirectory(Contact)
add_library(${LIBRARY}::Contacts ALIAS Contact)

add_subdirectory(Detection)
add_library(${LIBRARY}::CollisionDetection ALIAS CollisionDetection)

add_subdirectory(Policy)
add_library(${LIBRARY}::CollisionPolicy ALIAS CollisionPolicy)

add_subdirectory(Response)
add_library(${LIBRARY}::CollisionResponse ALIAS CollisionResponse)

add_subdirectory(ModuleConfiguration)
add_library(${LIBRARY}::ModuleConfiguration ALIAS CollisionSystemModuleConfiguration)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Contact
        CollisionDetection
        CollisionBatches
        CollisionResponse
        CollisionSystemConfiguration
        CollisionSystemModuleConfiguration
        Primitives
        ErrorHandling
        Utilities
)
