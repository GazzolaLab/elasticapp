# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

add_elastica_library(${LIBRARY})

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        Materials.cpp
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
add_subdirectory(Interface)
add_subdirectory(IO)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
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
        Thresholds.hpp
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
)
#set(LIBRARY_SOURCES Trigger.cpp Visualization.cpp Materials.cpp)

#target_include_directories(${LIBRARY} PRIVATE Interface)
#target_include_directories(${LIBRARY} PRIVATE CollisionSystem)
#target_include_directories(${LIBRARY} PRIVATE IO)

#add_elastica_library(${LIBRARY} ${LIBRARY_SOURCES})
#
#
