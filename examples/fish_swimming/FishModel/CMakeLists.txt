set(LIBRARY FishModel)

add_elastica_library(${LIBRARY} INTERFACE)

set(LIBRARY_SOURCES
    Tags.hpp
    Aliases.hpp
    Components.hpp
    Constraints.hpp
    Dissipations.hpp
    FishModel.hpp
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

add_subdirectory(Aliases)
add_library(${LIBRARY}::Aliases ALIAS FishModelAliases)

add_subdirectory(Components)
add_library(${LIBRARY}::Components ALIAS FishModelComponents)

add_subdirectory(Constraints)
add_library(${LIBRARY}::Components ALIAS FishModelConstraints)

# add_subdirectory(Connections)
# add_library(${LIBRARY}::Components ALIAS FishModelConnections)

add_subdirectory(Dissipations)
add_library(${LIBRARY}::Components ALIAS FishModelDissipations)

add_subdirectory(Python)
