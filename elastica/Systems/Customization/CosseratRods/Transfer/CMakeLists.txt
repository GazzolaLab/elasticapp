# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodTransferCustomization)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        Common.hpp
        CosseratRod.hpp
        CosseratRodWithoutDamping.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Common.hpp
        CosseratRod.hpp
        CosseratRodWithoutDamping.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CosseratRods
        Primitives
)
