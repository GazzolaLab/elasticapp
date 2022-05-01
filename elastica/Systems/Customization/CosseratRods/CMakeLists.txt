# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodsCustomization)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        common.hpp
        CosseratRod.hpp
        CosseratRodWithoutDamping.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PUBLIC
        common.hpp
        CosseratRod.hpp
        CosseratRodWithoutDamping.hpp
)

add_subdirectory(Common)
add_subdirectory(Geometry)
add_subdirectory(Transfer)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CosseratRodCommonCustomization
        CosseratRodGeometryCustomization
        CosseratRodTransferCustomization
)
