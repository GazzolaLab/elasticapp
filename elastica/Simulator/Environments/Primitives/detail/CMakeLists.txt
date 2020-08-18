set(LIBRARY Primitives)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        ContiguousPrimitive.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ContiguousPrimitive.hpp
)

