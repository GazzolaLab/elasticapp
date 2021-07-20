set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        AtWrapper.hpp
        QueryID.hpp
        Serialize.hpp
        ToWrapper.hpp
        UUID.hpp
        WithWrapper.hpp
        Wrappers.hpp
)
