# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

add_subdirectory(Aliases)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Aliases.hpp
        Connections.hpp
        Constraints.hpp
        Domain.hpp
        Environments.hpp
        Forcing.hpp
        Systems.hpp
)

#target_link_libraries(
#        ${LIBRARY}
#        PUBLIC
#        Utilities
#)
