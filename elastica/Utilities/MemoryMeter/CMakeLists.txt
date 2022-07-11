set(LIBRARY Utilities)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        MemoryMeter.hpp
        Tags.hpp
)
