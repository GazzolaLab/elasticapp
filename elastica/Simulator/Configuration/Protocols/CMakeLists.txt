# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorConfigurationProtocols)
set(DOMAIN_LIBRARY SimulatorConfigurationProtocolsDomain)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Connections.hpp
        Constraints.hpp
        Domain.hpp
        Environments.hpp
        Forcing.hpp
        Systems.hpp
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
        Utilities
        Stl
        SimulatorModuleConfigurationExecutionPolicy
        ${DOMAIN_LIBRARY}
)

# Domain protocols needs Frames and Domain as a dependency, so it is refactored as a separate dependency. 
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
        SimulatorModuleConfigurationExecutionPolicy
        Frames
        Domain
)
