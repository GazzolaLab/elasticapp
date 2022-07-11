# Distributed under the MIT License. See LICENSE.txt for details.

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Configuration.hpp
        IsInf.hpp
        IsNan.hpp
        IsPowerOf.hpp
        IsZero.hpp
)
