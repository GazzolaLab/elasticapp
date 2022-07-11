set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Configuration.hpp
        Serialize.hpp
        Simulator.hpp
        SystemsRegistrationHelpers.hpp
        Types.hpp
)
