# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        Trigger.cpp
        Visualization.cpp
)

add_subdirectory(Configuration)
target_link_libraries(${LIBRARY} INTERFACE SimulatorConfiguration)
add_library(${LIBRARY}::Configuration ALIAS SimulatorConfiguration)

add_subdirectory(Connections)
target_link_libraries(${LIBRARY} INTERFACE Connections)
add_library(${LIBRARY}::Connections ALIAS Connections)

add_subdirectory(Constraints)
target_link_libraries(${LIBRARY} INTERFACE Constraints)
add_library(${LIBRARY}::Constraints ALIAS Constraints)

add_subdirectory(Domain)
target_link_libraries(${LIBRARY} INTERFACE Domain)
add_library(${LIBRARY}::Domain ALIAS Domain)

add_subdirectory(Environments)
target_link_libraries(${LIBRARY} INTERFACE Environments)
add_library(${LIBRARY}::Environments ALIAS Environments)

add_subdirectory(Forcing)
target_link_libraries(${LIBRARY} INTERFACE Forcing)
add_library(${LIBRARY}::Forcing ALIAS Forcing)

add_subdirectory(Frames)
target_link_libraries(${LIBRARY} INTERFACE Frames)
add_library(${LIBRARY}::Frames ALIAS Frames)

add_subdirectory(Interface)
target_link_libraries(${LIBRARY} INTERFACE SimulatorInterface)
add_library(${LIBRARY}::Interface ALIAS SimulatorInterface)

# TODO : Refactor into separate module.
add_subdirectory(IO)

add_subdirectory(Materials)
add_library(elastica::Materials ALIAS Materials)

add_subdirectory(Utilities)
add_library(Simulator::Utilities ALIAS SimulatorUtilities)

set(LIBRARY_SOURCES
        Configuration.hpp
        Connections.hpp
        Constraints.hpp
        Domain.hpp
        EnableReset.hpp
        Environments.hpp
        Forcing.hpp
        Frames.hpp
        Indexers.hpp
        Materials.hpp
        Modules.hpp
        Protocols.hpp
        Simulator.hpp
        Types.hpp
        # TODO : Remove
        Trigger.hpp
        Visualization.hpp
)


elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ${LIBRARY_SOURCES}
)

# FIXME refactor serialization
target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Blaze
        ErrorHandling
        Utilities
        Parallel
        SimulatorConfiguration
        Connections
        Constraints
        Domain
        Environments
        Forcing
        Frames
        Materials
        SimulatorUtilities
        IO # This is a code smell. As our simulator imports systems/Block which in turn imports serialization, we need to include this at the moment
)
