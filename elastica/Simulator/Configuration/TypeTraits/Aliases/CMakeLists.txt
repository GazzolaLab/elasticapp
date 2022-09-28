# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorConfigurationTypeTraitsAliases)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Domain.hpp
        Environments.hpp
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