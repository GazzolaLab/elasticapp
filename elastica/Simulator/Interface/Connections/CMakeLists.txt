set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        Configuration.hpp
        Connecter.hpp
        ConnecterBase.hpp
        Connection.hpp
        ConnectionBase.hpp
        ConnectionUUID.hpp
        ExecutionPolicy.hpp
        Module.hpp
        Serialize.hpp
        Types.hpp
)
