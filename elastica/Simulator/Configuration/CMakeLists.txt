# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY Simulator)

add_subdirectory(Defaults)
add_subdirectory(ModuleConfiguration)
add_subdirectory(Protocols)
add_subdirectory(TypeTraits)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        ModuleConfiguration.hpp
        Defaults.hpp
        DefaultSimulatorConfiguration.hpp
        Protocols.hpp
        SimulatorConfiguration.hpp
        Types.hpp
        TypeTraits.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        DefaultSimulatorConfiguration.cpp
)
