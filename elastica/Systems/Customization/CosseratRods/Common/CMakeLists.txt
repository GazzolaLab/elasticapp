# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodCommonCustomization)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        PrimitiveTrait.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        PrimitiveTrait.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CosseratRods
        SystemsCommon
)
