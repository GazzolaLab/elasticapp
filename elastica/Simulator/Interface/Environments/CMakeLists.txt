set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Configuration.hpp
        Emplacer.hpp
        ExecutionPolicy.hpp
        WithImposedPrimitives.hpp
        WithImposedPrimitivesExecutionPolicy.hpp
        Module.hpp
        Serialize.hpp
        WithPrimitives.hpp
        WithPrimitivesExecutionPolicy.hpp
        Types.hpp
)
