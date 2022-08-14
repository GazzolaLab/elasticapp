# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY IO_Interface)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        FileNamingConvention.cpp
        Helpers.cpp
        WorkingDirectory.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        FileNamingConvention.hpp
        Helpers.hpp
        MetadataInterface.hpp
        Types.hpp
        WorkingDirectory.hpp
)

target_link_libraries(
        ${LIBRARY}
        PRIVATE
        Stl
        Utilities
)
