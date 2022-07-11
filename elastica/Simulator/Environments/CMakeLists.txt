# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Environments)

add_elastica_library(${LIBRARY} INTERFACE)

# include primitives first before collision
add_subdirectory(Common)
add_library(Environments::Common ALIAS EnvironmentsCommon)

add_subdirectory(CollisionSystem)
add_library(Environments::CollisionSystem ALIAS CollisionSystem)

add_subdirectory(Primitives)
add_library(Environments::Primitives ALIAS Primitives)

add_subdirectory(GravityField)
add_library(Environments::GravityField ALIAS GravityField)

add_subdirectory(Kinematics)
add_library(Environments::Kinematics ALIAS Kinematics)

add_subdirectory(ModuleConfiguration)
add_library(Environments::ModuleConfiguration ALIAS EnvironmentsModuleConfiguration)

add_subdirectory(Transfer)
add_library(Environments::Transfer ALIAS Transfer)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        Common.hpp
        CollisionSystem.hpp
        Environments.hpp
        GravityField.hpp
        Primitives.hpp
        Protocols.hpp
        Tags.hpp
        Transfer.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Common.hpp
        CollisionSystem.hpp
        Environments.hpp
        GravityField.hpp
        Primitives.hpp
        Protocols.hpp
        Tags.hpp
        Transfer.hpp
        Types.hpp
        TypeTraits.hpp
)


target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CollisionSystem
        Primitives
        GravityField
        EnvironmentsModuleConfiguration
        Transfer
        EnvironmentsCommon
)
