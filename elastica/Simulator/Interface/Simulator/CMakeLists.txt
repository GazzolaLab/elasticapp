set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Configuration.hpp
        Profiling.hpp
        Serialize.hpp
        Simulator.hpp
        Types.hpp
)
