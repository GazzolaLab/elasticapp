set(LIBRARY ConstraintsInterfaceCore)
set(LIBRARY_EXECUTIONPOLICY ConstraintsInterfaceExecutionPolicy)
set(LIBRARY_BATCHINGPOLICY ConstraintsInterfaceBatchingPolicy)
set(LIBRARY_SERIALIZATION ConstraintsInterfaceSerialization)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Configuration.hpp
        Constrainer.hpp
        ConstrainerBase.hpp
        Constraint.hpp
        ConstraintBase.hpp
        ConstraintUUID.hpp
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
        Constraints
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
        ConstraintsInterfaceCore
        InterfaceWrappersSerialization
        Utilities
        Stl
)


set(LIBRARY ConstraintsInterface)
add_elastica_library(${LIBRARY} INTERFACE)
target_link_libraries(
        ${LIBRARY}
        INTERFACE
        ConstraintsInterfaceCore
        ${LIBRARY_EXECUTIONPOLICY}
        ${LIBRARY_BATCHINGPOLICY}
)

