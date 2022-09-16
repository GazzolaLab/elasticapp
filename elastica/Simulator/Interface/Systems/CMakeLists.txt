set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        BlockRefs.hpp
        ExecutionPolicy.hpp
        Module.hpp
        Serialize.hpp
        Types.hpp
)
