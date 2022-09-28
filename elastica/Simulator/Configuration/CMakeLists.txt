# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorConfigurationInternal)

add_elastica_library(${LIBRARY})

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Defaults.hpp
        DefaultSimulatorConfiguration.hpp
        ModuleConfiguration.hpp
        Protocols.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        DefaultSimulatorConfiguration.cpp
)

add_subdirectory(Defaults)
add_library(${LIBRARY}::Defaults ALIAS SimulatorConfigurationDefaults)

add_subdirectory(ModuleConfiguration)
add_library(${LIBRARY}::ModuleConfiguration ALIAS SimulatorModuleConfiguration)

add_subdirectory(Protocols)
add_library(${LIBRARY}::Protocols ALIAS SimulatorConfigurationProtocols)

add_subdirectory(TypeTraits)
add_library(${LIBRARY}::TypeTraits ALIAS SimulatorConfigurationTypeTraits)


target_link_libraries(
        ${LIBRARY}
        PUBLIC
        SimulatorConfigurationDefaults
        SimulatorModuleConfiguration
        SimulatorConfigurationProtocols
        SimulatorConfigurationTypeTraits
        Utilities
        Stl
)

set(LIBRARY SimulatorConfigurationCore)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        SimulatorConfiguration.hpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        SimulatorConfigurationDefaults
        SimulatorModuleConfiguration
        SimulatorConfigurationProtocols
        SimulatorConfigurationTypeTraits
        Stl
        Time
        Systems
)

set(LIBRARY SimulatorConfiguration)

add_elastica_library(${LIBRARY} INTERFACE)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        SimulatorConfigurationInternal
        SimulatorConfigurationCore
)

