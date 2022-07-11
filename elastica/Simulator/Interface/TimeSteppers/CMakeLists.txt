# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        ExplicitSteppers.hpp
        Metadata.hpp
        SymplecticSteppers.hpp
        Types.hpp
)
