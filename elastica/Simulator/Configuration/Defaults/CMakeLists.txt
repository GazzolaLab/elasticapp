# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Connections.hpp
        Constraints.hpp
        Domain.hpp
        Environments.hpp
        Forcing.hpp
        Parallel.hpp
        Systems.hpp
        Types.hpp
)

#target_link_libraries(
#        ${LIBRARY}
#        PUBLIC
#        Utilities
#)
