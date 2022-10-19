# Distributed under the MIT License. See LICENSE.txt for details.

# TODO Refactor IO into a separate module depending on the Systems module (not Simulator)
set(LIBRARY IO)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Interface)
add_library(${LIBRARY}::Interface ALIAS IO_Interface)

add_subdirectory(Serialization)
add_library(${LIBRARY}::Serialization ALIAS Serialization)

set(LIBRARY_SOURCES
        Serialization.hpp
        Types.hpp
        )

add_subdirectory(H5)
if (TARGET H5)
    add_library(${LIBRARY}::H5 ALIAS H5)
    LIST(APPEND LIBRARY_SOURCES "H5.hpp")
    target_link_libraries(
            ${LIBRARY}
            INTERFACE
            H5)
endif ()

add_subdirectory(YAML)
if (TARGET YAMLInterface)
    add_library(${LIBRARY}::YAML ALIAS YAMLInterface)
    LIST(APPEND LIBRARY_SOURCES "YAML.hpp")
    target_link_libraries(
            ${LIBRARY}
            INTERFACE
            YAMLInterface)
endif ()

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ${LIBRARY_SOURCES}
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Simulator # For Triggers and visualization
        Serialization
        # PRIVATE
        # std::filesystem
        #        -stdlib=libc++
        #        stdc++fs
)

