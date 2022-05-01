# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY CosseratRodComponents)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Components.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        WithRodKinematics.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Components.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        WithRodKinematics.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Materials
)
