set(LIBRARY Simulator)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}/elastica
        HEADERS
        Configuration.hpp
        Forcer.hpp
        ForcerBase.hpp
        Forcing.hpp
        ForcingBase.hpp
        ForcingUUID.hpp
        ExecutionPolicy.hpp
        Module.hpp
        Serialize.hpp
        Types.hpp
)
