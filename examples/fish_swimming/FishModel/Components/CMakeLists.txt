set(LIBRARY FishModelComponents)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
    WithFishGeometry.hpp
)
elastica_target_headers(
    ${LIBRARY}
    INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/examples
    ${LIBRARY_SOURCES}
)

elastica_target_sources(
    ${LIBRARY}
    INTERFACE
    ${LIBRARY_SOURCES}
)