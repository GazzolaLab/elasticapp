set(LIBRARY MagnetismExtensionFieldProfiles)

add_elastica_library(${LIBRARY} INTERFACE)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        ConstantMagneticField.hpp
        RampedAdapter.hpp
        Rampup.hpp
)

elastica_target_sources(
        ${LIBRARY}
        INTERFACE
        ConstantMagneticField.hpp
        RampedAdapter.hpp
        Rampup.hpp
)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        Utilities
        Options
        Time
)
