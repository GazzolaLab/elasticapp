# Distributed under the MIT License. See LICENSE.txt for details.

set(LIBRARY StateTypeTraits)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        Systems
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Aliases.hpp
        HasOrder.hpp
        IsExpression.hpp
        IsPrimitive.hpp
        IsScalar.hpp
        IsVectorized.hpp
        SupportsVectorizedOperations.hpp
)

elastica_target_sources(
        Systems
        INTERFACE
        Aliases.hpp
        HasOrder.hpp
        IsExpression.hpp
        IsPrimitive.hpp
        IsScalar.hpp
        IsVectorized.hpp
        SupportsVectorizedOperations.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        StateExpressions
)

