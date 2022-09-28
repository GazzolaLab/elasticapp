# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SystemsInterfaceCore)
set(LIBRARY_EXECUTIONPOLICY SystemsInterfaceExecutionPolicy)
set(LIBRARY_SERIALIZATION SystemsInterfaceSerialization)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        BlockRefs.hpp
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
        SimulatorConfiguration
        ModuleSettings # profiling
        Systems
        ${LIBRARY_EXECUTIONPOLICY}
)


set(LIBRARY ${LIBRARY_EXECUTIONPOLICY})
add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES 
        ExecutionPolicy.hpp
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
        Systems
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
        SystemsInterfaceCore
        SimulatorModuleConfigurationSerialize
        Utilities
        Stl
)


set(LIBRARY SystemsInterface)
add_elastica_library(${LIBRARY} INTERFACE)
target_link_libraries(
        ${LIBRARY}
        INTERFACE
        SystemsInterfaceCore
        ${LIBRARY_EXECUTIONPOLICY}
        ${LIBRARY_BATCHINGPOLICY}
)

