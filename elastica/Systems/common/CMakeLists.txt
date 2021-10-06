# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Access.hpp
        Protocols.hpp
        SymplecticStepperAdapter.hpp
        Tags.hpp
)

