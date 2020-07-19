# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Environments)

add_elastica_library(${LIBRARY} INTERFACE)
#add_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Tags.hpp
        TypeTraits.hpp
)

# include primitives first before collision
add_subdirectory(Primitives)
add_subdirectory(CollisionSystem)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        #        $<TARGET_OBJECTS:CollisionSystem>
        CollisionSystem
        Primitives
        Simulator # For Materials and Types
        ErrorHandling
        Utilities
)

