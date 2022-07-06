# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SerializationSTL)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
        array.hpp
        associative_containers.hpp
        map.hpp
        string.hpp
        tuple.hpp
        unordered_map.hpp
        utility.hpp
        vector.hpp
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

# TODO : add STL target from #350
#target_link_libraries(
#        ${LIBRARY}
#        INTERFACE
#        Stl
#)
