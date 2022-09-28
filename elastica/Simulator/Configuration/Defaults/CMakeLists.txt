# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorConfigurationDefaults)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Connections.hpp
        Constraints.hpp
        Domain.hpp
        Environments.hpp
        Forcing.hpp
        Parallel.hpp
        Systems.hpp
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
        Utilities
        SimulatorModuleConfiguration
)

