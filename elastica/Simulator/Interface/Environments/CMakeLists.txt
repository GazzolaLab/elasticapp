# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY EnvironmentsInterfaceCore)
set(LIBRARY_EXECUTIONPOLICY EnvironmentsInterfaceExecutionPolicy)
set(LIBRARY_SERIALIZATION EnvironmentsInterfaceSerialization)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        Configuration.hpp
        Emplacer.hpp
        Module.hpp
        WithImposedPrimitives.hpp
        WithPrimitives.hpp
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
        ModuleSettings # profiling
        SimulatorUtilities
        Environments
        ${LIBRARY_EXECUTIONPOLICY}
)


set(LIBRARY ${LIBRARY_EXECUTIONPOLICY})
add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES 
        ExecutionPolicy.hpp
        WithImposedPrimitivesExecutionPolicy.hpp
        WithPrimitivesExecutionPolicy.hpp
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
        Environments
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
        EnvironmentsInterfaceCore
        Utilities
        Stl
)


set(LIBRARY EnvironmentsInterface)
add_elastica_library(${LIBRARY} INTERFACE)
target_link_libraries(
        ${LIBRARY}
        INTERFACE
        EnvironmentsInterfaceCore
        ${LIBRARY_EXECUTIONPOLICY}
        ${LIBRARY_BATCHINGPOLICY}
)

