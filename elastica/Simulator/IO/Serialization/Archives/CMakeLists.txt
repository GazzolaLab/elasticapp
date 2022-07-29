# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SerializationArchives)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        base64.hpp
        Binary.hpp
        Types.hpp
        YAML.hpp
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
        Utilities
        Stl
)

if (YamlCpp_FOUND)
    target_link_libraries(${LIBRARY} INTERFACE YamlCpp)
endif ()
