set(LIBRARY ConnectionsInterfaceCore)
set(LIBRARY_EXECUTIONPOLICY ConnectionsInterfaceExecutionPolicy)
set(LIBRARY_SERIALIZATION ConnectionsInterfaceSerialization)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Configuration.hpp
        Connecter.hpp
        ConnecterBase.hpp
        Connection.hpp
        ConnectionBase.hpp
        ConnectionUUID.hpp
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
        Connections
        ${LIBRARY_EXECUTIONPOLICY}
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
        ConnectionsInterfaceCore
        InterfaceWrappersSerialization
        Utilities
        Stl
)


set(LIBRARY ConnectionsInterface)
add_elastica_library(${LIBRARY} INTERFACE)
target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ConnectionsInterfaceCore
        ${LIBRARY_EXECUTIONPOLICY}
)

