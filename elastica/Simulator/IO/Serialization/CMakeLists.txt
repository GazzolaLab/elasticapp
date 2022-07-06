# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Serialization)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(Archives)
add_library(${LIBRARY}::Archives ALIAS SerializationArchives)

# TODO : refactor out of serialization
add_subdirectory(Custom)
add_library(${LIBRARY}::Custom ALIAS SerializationCustom)

add_subdirectory(STL)
add_library(${LIBRARY}::STL ALIAS SerializationSTL)

set(LIBRARY_SOURCES
        Archives.hpp
        Custom.hpp # TODO : refactor out of serialization
        Helpers.hpp
        Serialization.hpp
        Serialize.hpp
        SerializeWizard.hpp
        STL.hpp
        Types.hpp
        )

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ${LIBRARY_SOURCES}
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ${LIBRARY_SOURCES}
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        SerializationArchives
        SerializationCustom
        SerializationSTL
        Utilities
)
