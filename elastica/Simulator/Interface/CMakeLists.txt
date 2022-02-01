# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

add_subdirectory(Affordances)
add_subdirectory(Batching)
add_subdirectory(Connections)
add_subdirectory(Constraints)
#add_subdirectory(Environments) # TODO: enable later
add_subdirectory(Forcing)
add_subdirectory(Indexers)
add_subdirectory(Protocols)
add_subdirectory(Systems)
add_subdirectory(Simulator)
add_subdirectory(TypeTraits)
add_subdirectory(TimeSteppers)
add_subdirectory(Wrappers)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Connections.hpp
        Constraints.hpp
        # Environments.hpp
        Forcing.hpp
        Indexers.hpp
        Protocols.hpp
        Simulator.hpp
        Serialize.hpp
        Types.hpp
        TypeTraits.hpp
)

