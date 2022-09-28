# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY ForcingInterfaceCore)
set(LIBRARY_EXECUTIONPOLICY ForcingInterfaceExecutionPolicy)
set(LIBRARY_BATCHINGPOLICY ForcingInterfaceBatchingPolicy)
set(LIBRARY_SERIALIZATION ForcingInterfaceSerialization)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Configuration.hpp
        Forcer.hpp
        ForcerBase.hpp
        Forcing.hpp
        ForcingBase.hpp
        ForcingUUID.hpp
        Module.hpp
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
        Stl
        Utilities
        Time
        SimulatorConfiguration
        InterfaceIndexers
        InterfaceProtocols
        InterfaceWrappers
        Forcing
        ${LIBRARY_EXECUTIONPOLICY}
        ${LIBRARY_BATCHINGPOLICY}
)


set(LIBRARY ${LIBRARY_EXECUTIONPOLICY})
add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES ExecutionPolicy.hpp)

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
        SimulatorConfiguration
        InterfaceAffordances
        Stl
        Utilities
        Parallel
)

set(LIBRARY ${LIBRARY_BATCHINGPOLICY})
add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES BatchingPolicy.hpp)

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
        InterfaceBatching
        Utilities
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
        Serialization
        ForcingInterfaceCore
        InterfaceWrappersSerialization
        Utilities
        Stl
)


set(LIBRARY ForcingInterface)
add_elastica_library(${LIBRARY} INTERFACE)
target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ForcingInterfaceCore
        ${LIBRARY_EXECUTIONPOLICY}
        ${LIBRARY_BATCHINGPOLICY}
)

