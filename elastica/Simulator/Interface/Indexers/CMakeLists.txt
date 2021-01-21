# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        At.hpp
        Pair.hpp
        Range.hpp
        Types.hpp
        TypeTraits.hpp
)
