# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CollisionSystemModuleConfiguration)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        ModuleConfiguration.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ModuleConfiguration.hpp
        Types.hpp
)

# TODO : refactor this to depend on configuration
#target_link_libraries(
#        ${LIBRARY}
#        INTERFACE
#        UnionFindExecutionPolicy
#        Utilities
#        Primitives
#        Contact
#)
