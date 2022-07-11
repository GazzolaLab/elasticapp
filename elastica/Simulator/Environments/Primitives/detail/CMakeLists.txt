set(LIBRARY Primitives)

elastica_target_sources(
        ${LIBRARY}
        PRIVATE
        CapsulePrimitiveBase.cpp
        # ContiguousPrimitive.cpp
)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        CapsulePrimitiveBase.hpp
        # ContiguousPrimitive.hpp
)

