# Distributed under the MIT License. See LICENSE.txt for details.

if (YamlCpp_FOUND)
    set(LIBRARY YAMLInterface)

    add_elastica_library(${LIBRARY})

    elastica_target_sources(
            ${LIBRARY}
            PRIVATE
            YAMLMetadata.cpp
    )

    elastica_target_headers(
            ${LIBRARY}
            INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
            HEADERS
            YAMLMetadata.hpp
            Types.hpp
    )

    target_link_libraries(
            ${LIBRARY}
            PUBLIC
            SerializationArchives
            YamlCpp
            IO_Interface
            Utilities
    )
endif ()
