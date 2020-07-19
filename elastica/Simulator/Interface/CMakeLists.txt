set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Connections.hpp
        Constraints.hpp
        Environments.hpp
        Simulator.hpp
        DefaultSimulatorConfiguration.hpp
)
