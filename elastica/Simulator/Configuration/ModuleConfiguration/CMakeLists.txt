# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorModuleConfiguration)
set(DOMAIN_LIBRARY SimulatorModuleConfigurationDomain)
set(ENVIRONMENTS_LIBRARY SimulatorModuleConfigurationEnvironments)
set(EXECUTION_LIBRARY SimulatorModuleConfigurationExecutionPolicy)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        BlockingPolicy.hpp
        Connections.hpp
        Constraints.hpp
        Environments.hpp
        Forcing.hpp
        IterationPolicy.hpp
        Parallel.hpp
        Streams.hpp
        Systems.hpp # Requires types
        Types.hpp
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

# Only types from other modules are required, so those dependencies
# are skipped.
target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Stl
        Utilities
        SimulatorConfigurationProtocols
        ModuleSettings
        ${DOMAIN_LIBRARY}
        ${ENVIRONMENTS_LIBRARY}
        ${EXECUTION_LIBRARY}
)


# Domain Configuration needs Frames and Domain as a dependency, so it is refactored as a separate dependency. 
set(LIBRARY ${DOMAIN_LIBRARY})

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES Domain.hpp)

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
        Stl
        Utilities
        SimulatorConfigurationProtocolsDomain
        Frames
        Domain
)


# Environments may need Environment as a dependency, so it is refactored as a separate dependency. 
set(LIBRARY ${ENVIRONMENTS_LIBRARY})

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES Environments.hpp)

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
        Utilities
        SimulatorConfigurationProtocols
        # TODO : Enable after types declared.
        # EnvironmentTypes
)

# Some aspects of ExecutionPolicy is used by the Protocols. To keep dependencies
# clean, it is refactored into a separate module.
set(LIBRARY ${EXECUTION_LIBRARY})

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES ExecutionPolicy.hpp)

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
        Utilities
        Stl
)


set(LIBRARY SimulatorModuleConfigurationSerialize)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES Serialize.hpp)

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
        Utilities
        Stl
        Serialization
        SimulatorModuleConfiguration
)
