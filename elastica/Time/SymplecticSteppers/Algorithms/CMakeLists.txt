set(LIBRARY Time)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        PositionVerlet.hpp
        Types.hpp
        VelocityVerlet.hpp
)
