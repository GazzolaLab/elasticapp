set(LIBRARY MagneticEnvironment)

add_elastica_library(${LIBRARY} INTERFACE)

add_subdirectory(MagneticField)
add_library(MagneticEnvironment::MagneticField ALIAS MagneticField)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        MagneticField.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        MagneticField.hpp
)


target_link_libraries(
        ${LIBRARY}
        INTERFACE
        MagneticField
        Environments
)

