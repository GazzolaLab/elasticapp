# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SimulatorCore)
set(LIBRARY_SERIALIZATION SimulatorCoreSerialization)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Configuration.hpp
        Simulator.hpp
        SystemsRegistrationHelpers.hpp
        Types.hpp
        Types.hpp
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

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ErrorHandling
        SimulatorConfiguration
        Utilities
        Stl
        Parallel
        InterfaceBatching
        InterfaceTypeTraits
        SystemsInterface
        InterfaceTimeSteppers
        SimulatorUtilities
)


set(LIBRARY ${LIBRARY_SERIALIZATION})
add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES Serialize.hpp)

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

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ErrorHandling
        Serialization
        SimulatorCore
        Domain # TODO : Domain Serialization
        # ConnectionsInterfaceSerialization
        # ConstraintsInterfaceSerialization
        # EnvironmentsInterfaceSerialization
        # ForcingInterfaceSerialization
        # InterfaceWrappersSerialization
        SystemsInterfaceSerialization
        Utilities
        Stl
)
