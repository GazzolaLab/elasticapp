# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY MagneticFieldModuleConfiguration)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ModuleConfiguration.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PUBLIC
        ModuleConfiguration.hpp
        Types.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
)
