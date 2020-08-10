set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Connections.hpp
        Constraints.hpp
        DefaultSimulatorConfiguration.hpp
        Environments.hpp
        Simulator.hpp
        SimulatorConfiguration.hpp
)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        DefaultSimulatorConfiguration.cpp
)

add_subdirectory(TypeTraits)

#target_link_libraries(
#        ${LIBRARY}
#        PUBLIC
#        ErrorHandling
#        Utilities
#)
