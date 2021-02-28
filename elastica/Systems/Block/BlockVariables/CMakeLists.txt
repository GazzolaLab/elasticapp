# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Aliases.hpp
        BlockInitializer.hpp
        BlockVariables.hpp
        Protocols.hpp
        Types.hpp
        TypeTraits.hpp
)
