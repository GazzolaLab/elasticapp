# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY SystemsCommon)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Access.hpp
        Protocols.hpp
        SymplecticStepperAdapter.hpp
        Tags.hpp
        Types.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Access.hpp
        Components.hpp
        Protocols.hpp
        SymplecticStepperAdapter.hpp
        Tags.hpp
        Types.hpp
)

add_subdirectory(Components)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Components
)
