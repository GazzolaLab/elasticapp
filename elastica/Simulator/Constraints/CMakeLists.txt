# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Constraint.hpp
        MakeConstraint.hpp
        Others.hpp
        Protocols.hpp
        Registration.hpp
        Types.hpp
)
