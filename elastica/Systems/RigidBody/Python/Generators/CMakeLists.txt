# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "PyGeneratorsRigidBody")

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        BindMemberAccess.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        BindMemberAccess.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        RigidBody
        Utilities
        PyGeneratorsCosseratRod
)
