set(LIBRARY FishModelDissipations)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
    AnalyticalLinearDamper.hpp
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