# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

add_subdirectory(Aliases)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Aliases.hpp
        Domain.hpp
        Environments.hpp
        Simulator.hpp
)
