add_subdirectory(detail)

set(LIBRARY Systems)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Components.hpp
        Protocols.hpp
        Tags.hpp
        Types.hpp
        WithCircleCrossSection.hpp
        WithSquareCrossSection.hpp
)
