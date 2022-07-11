set(LIBRARY Utilities)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        Orthogonal.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Accuracy.hpp
        Dot.hpp
        Equal.hpp
        Identity.hpp
        Infinity.hpp
        Invert.hpp
        Limits.hpp
        Length.hpp
        Normalize.hpp
        Orthogonal.hpp
        Rot3.hpp
        SqrLength.hpp
        Square.hpp
        Trans.hpp
        Vec3.hpp
        Zero.hpp
)

add_subdirectory(Python)