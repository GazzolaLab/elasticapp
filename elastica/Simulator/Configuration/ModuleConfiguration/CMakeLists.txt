# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Connections.hpp
        Constraints.hpp
        Domain.hpp
        Environments.hpp
        Forcing.hpp
        Systems.hpp
        Types.hpp
)
