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

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        BoundaryConditions.hpp
        Environments.hpp
        Joints.hpp
        Materials.hpp
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
)
#set(LIBRARY_SOURCES Trigger.cpp Visualization.cpp Materials.cpp)

#target_include_directories(${LIBRARY} PRIVATE Interface)
#target_include_directories(${LIBRARY} PRIVATE CollisionSystem)
#target_include_directories(${LIBRARY} PRIVATE IO)

add_subdirectory(Environments)
add_subdirectory(IO)
add_subdirectory(Interface)

#add_elastica_library(${LIBRARY} ${LIBRARY_SOURCES})
#
#
