set(LIBRARY MagnetismExtension)

add_elastica_library(${LIBRARY} INTERFACE)
# include as "Extension/..."
target_include_directories(${LIBRARY} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})
# include as <elastica/...>
target_include_directories(${LIBRARY} INTERFACE ${CMAKE_SOURCE_DIR})

add_subdirectory(Extension)
add_subdirectory(FieldProfiles)

target_link_libraries(
        ${LIBRARY}
        INTERFACE
        MagnetismExtensionSystems
        MagnetismExtensionEnvironments
        MagnetismExtensionFieldProfiles
)

add_subdirectory(ConstantMagneticFieldCaseStudy)
add_subdirectory(OscillatoryMagneticFieldCaseStudy)
add_subdirectory(OscillatoryMagneticCiliaCarpetCaseStudy)
