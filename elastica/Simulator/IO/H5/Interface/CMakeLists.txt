# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY H5Interface)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        H5Metadata.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        H5Metadata.hpp
        H5Interface.hpp
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        HighFive
        # PRIVATE
        IO_Interface
        # FIXME : for some reason without Utilities Cmake does not pick up high five
        # https://github.com/tp5uiuc/elasticapp/issues/415
        Utilities
)
