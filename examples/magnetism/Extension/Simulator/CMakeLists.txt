add_subdirectory(Environments)

set(LIBRARY MagnetismExtensionEnvironments)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        Environments.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        Environments.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        MagneticEnvironment
        Environments
)
