# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorConfigurationTypeTraits)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Aliases.hpp
        Connections.hpp
        Constraints.hpp
        Domain.hpp
        Environments.hpp
        Forcing.hpp
        Systems.hpp
)

add_subdirectory(Aliases)
target_link_libraries(${LIBRARY} INTERFACE SimulatorConfigurationTypeTraitsAliases)
add_library(${LIBRARY}::Aliases ALIAS SimulatorConfigurationTypeTraitsAliases)

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
)