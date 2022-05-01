# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY GravityField)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(ModuleConfiguration)
add_library(${LIBRARY}::ModuleConfiguration ALIAS GravityFieldModuleConfiguration)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        GravityField.hpp
        Tags.hpp
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        # needed tags
        Systems
        Time
        EnvironmentsCommon
        GravityFieldModuleConfiguration
)


