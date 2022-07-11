set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Configuration.hpp
        Constrainer.hpp
        ConstrainerBase.hpp
        Constraint.hpp
        ConstraintBase.hpp
        ConstraintUUID.hpp
        ExecutionPolicy.hpp
        Module.hpp
        Serialize.hpp
        Types.hpp
)
