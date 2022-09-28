# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorInterface)
add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Affordances)
target_link_libraries(${LIBRARY} INTERFACE InterfaceAffordances)
add_library(${LIBRARY}::Affordances ALIAS InterfaceAffordances)

add_subdirectory(Batching)
target_link_libraries(${LIBRARY} INTERFACE InterfaceBatching)
add_library(${LIBRARY}::Batching ALIAS InterfaceBatching)

add_subdirectory(Connections)
target_link_libraries(${LIBRARY} INTERFACE ConnectionsInterface)
add_library(${LIBRARY}::Connections ALIAS ConnectionsInterface)

add_subdirectory(Constraints)
target_link_libraries(${LIBRARY} INTERFACE ConstraintsInterface)
add_library(${LIBRARY}::Constraints ALIAS ConstraintsInterface)

add_subdirectory(Environments)
target_link_libraries(${LIBRARY} INTERFACE EnvironmentsInterface)
add_library(${LIBRARY}::Environments ALIAS EnvironmentsInterface)

add_subdirectory(Forcing)
target_link_libraries(${LIBRARY} INTERFACE ForcingInterface)
add_library(${LIBRARY}::Forcing ALIAS ForcingInterface)

add_subdirectory(Indexers)
target_link_libraries(${LIBRARY} INTERFACE InterfaceIndexers)
add_library(${LIBRARY}::Indexers ALIAS InterfaceIndexers)

add_subdirectory(Protocols)
target_link_libraries(${LIBRARY} INTERFACE InterfaceProtocols)
add_library(${LIBRARY}::Protocols ALIAS InterfaceProtocols)

add_subdirectory(Simulator)
target_link_libraries(${LIBRARY} INTERFACE SimulatorCore)
add_library(${LIBRARY}::Core ALIAS SimulatorCore)

add_subdirectory(Systems)
target_link_libraries(${LIBRARY} INTERFACE SystemsInterface)
add_library(${LIBRARY}::Systems ALIAS SystemsInterface)

add_subdirectory(TypeTraits)
target_link_libraries(${LIBRARY} INTERFACE InterfaceTypeTraits)
add_library(${LIBRARY}::TypeTraits ALIAS InterfaceTypeTraits)

add_subdirectory(TimeSteppers)
target_link_libraries(${LIBRARY} INTERFACE InterfaceTimeSteppers)
add_library(${LIBRARY}::TimeSteppers ALIAS InterfaceTimeSteppers)

add_subdirectory(Wrappers)
target_link_libraries(${LIBRARY} INTERFACE InterfaceWrappers)
add_library(${LIBRARY}::Wrappers ALIAS InterfaceWrappers)

set(LIBRARY_SOURCES
        Connections.hpp
        Constraints.hpp
        Environments.hpp
        Forcing.hpp
        Indexers.hpp
        Protocols.hpp
        Simulator.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        ${LIBRARY_SOURCES}
)

set(LIBRARY SimulatorInterfaceSerialization)
add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Serialize.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        ${LIBRARY_SOURCES}
)

target_link_libraries(${LIBRARY} INTERFACE ConnectionsInterfaceSerialization)
add_library(${LIBRARY}::Connections ALIAS ConnectionsInterfaceSerialization)

target_link_libraries(${LIBRARY} INTERFACE ConstraintsInterfaceSerialization)
add_library(${LIBRARY}::Constraints ALIAS ConstraintsInterfaceSerialization)

target_link_libraries(${LIBRARY} INTERFACE EnvironmentsInterfaceSerialization)
add_library(${LIBRARY}::Environments ALIAS EnvironmentsInterfaceSerialization)

target_link_libraries(${LIBRARY} INTERFACE ForcingInterfaceSerialization)
add_library(${LIBRARY}::Forcing ALIAS ForcingInterfaceSerialization)

target_link_libraries(${LIBRARY} INTERFACE SimulatorCoreSerialization)
add_library(${LIBRARY}::SimulatorCore ALIAS SimulatorCoreSerialization)

target_link_libraries(${LIBRARY} INTERFACE SystemsInterfaceSerialization)
add_library(${LIBRARY}::Systems ALIAS SystemsInterfaceSerialization)

target_link_libraries(${LIBRARY} INTERFACE InterfaceWrappersSerialization)
add_library(${LIBRARY}::Wrappers ALIAS InterfaceWrappersSerialization)
