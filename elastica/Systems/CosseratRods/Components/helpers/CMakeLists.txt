# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodComponents)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        AdapterHelpers.hpp
        Component.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        AdapterHelpers.hpp
        Component.hpp
        Types.hpp
        TypeTraits.hpp
)
