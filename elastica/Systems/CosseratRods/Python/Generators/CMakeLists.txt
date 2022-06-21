# Distributed under the MIT License.
# See LICENSE.txt for details.

set(LIBRARY "PyGeneratorsCosseratRod")

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        BindEnergyMethods.hpp
        BindMemberAccess.hpp
        BindPyElasticaInterface.hpp
        BoundChecks.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        BindEnergyMethods.hpp
        BindMemberAccess.hpp
        BindPyElasticaInterface.hpp
        BoundChecks.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        CosseratRods
        Utilities
        ConvertCase #
        PySystemsCommon #
)
