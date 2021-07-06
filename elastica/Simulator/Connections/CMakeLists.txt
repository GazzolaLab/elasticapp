# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Connection.hpp
        MakeConnection.hpp
        Registration.hpp
        Protocols.hpp
        Types.hpp
)
