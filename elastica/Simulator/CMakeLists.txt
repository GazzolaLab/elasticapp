# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        Trigger.cpp
        Visualization.cpp
)

add_subdirectory(Configuration)
add_subdirectory(Connections)
add_subdirectory(Constraints)
add_subdirectory(Domain)
add_subdirectory(Environments)
add_subdirectory(Forcing)

add_subdirectory(Frames)
target_link_libraries(${LIBRARY} INTERFACE Frames)
add_library(${LIBRARY}::Frames ALIAS Frames)

add_subdirectory(Interface)
add_subdirectory(IO)

add_subdirectory(Materials)
add_library(elastica::Materials ALIAS Materials)

add_subdirectory(Utilities)
add_library(Simulator::Utilities ALIAS SimulatorUtilities)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        Configuration.hpp
        Connections.hpp
        Constraints.hpp
        Domain.hpp
        Environments.hpp
        Forcing.hpp
        Frames.hpp
        Indexers.hpp
        Materials.hpp
        Modules.hpp
        Protocols.hpp
        Simulator.hpp
        Trigger.hpp
        Types.hpp
        Visualization.hpp
)

target_link_libraries(
        ${LIBRARY}
        PUBLIC
        Blaze
        ErrorHandling
        Utilities
        Domain
        Frames
        Parallel
        SimulatorUtilities
        Materials
)
#set(LIBRARY_SOURCES Trigger.cpp Visualization.cpp Materials.cpp)

#target_include_directories(${LIBRARY} PRIVATE Interface)
#target_include_directories(${LIBRARY} PRIVATE CollisionSystem)
#target_include_directories(${LIBRARY} PRIVATE IO)

#add_elastica_library(${LIBRARY} ${LIBRARY_SOURCES})
#
#
