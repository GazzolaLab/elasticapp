# Distributed under the MIT License. See LICENSE.txt for details.

include(CMakePackageConfigHelpers)

if (WIN32 AND NOT CYGWIN)
    set(ELASTICA_INSTALL_CONFIGDIR CMake)
else ()
    set(ELASTICA_INSTALL_CONFIGDIR ${CMAKE_INSTALL_LIBDIR}/cmake/${PROJECT_NAME})
endif ()

get_property(PKG_EXTERNAL_DEPS GLOBAL PROPERTY ELASTICA_THIRD_PARTY_LIBS)
# Because we usually have custom targets for each imported library, here I "ship" the
# Setup${LIBRARYNAME} and Find${LIBRARYNAME} scripts to cmake as well
# https://stackoverflow.com/a/47704023

# First get "roots" of all libraries and export them, may need other variables too
# list(LENGTH PKG_EXTERNAL_DEPS N_EXTERNAL_DEPS)
#
# list(TRANSFORM PKG_EXTERNAL_DEPS TOUPPER OUTPUT_VARIABLE EXTERNAL_PKG_ROOT_TEMPLATES)
# list(TRANSFORM EXTERNAL_PKG_ROOT_TEMPLATES PREPEND "set(")
# list(TRANSFORM EXTERNAL_PKG_ROOT_TEMPLATES APPEND "_ROOT")
#
# set(EXTERNAL_PKG_ROOTS "")
# foreach (I_EXT_DEP RANGE ${N_EXTERNAL_DEPS})
#     list(GET EXTERNAL_PKG_ROOT_TEMPLATES ${I_EXT_DEP} EXT_DEP_ROOT_TEMPLATE)
#     list(GET PKG_EXTERNAL_DEPS ${I_EXT_DEP} EXT_DEP)
#     list(APPEND EXTERNAL_PKG_ROOTS "${EXT_DEP_ROOT_TEMPLATE} ${${EXT_DEP}_ROOT}")
# endforeach ()

# list(TRANSFORM PKG_EXTERNAL_DEPS PREPEND "find_dependency(" OUTPUT_VARIABLE FIND_EXTERNAL_PKG_DEPENDENCIES)
# list(TRANSFORM FIND_EXTERNAL_PKG_DEPENDENCIES APPEND " MODULE REQUIRED)")
# list(JOIN FIND_EXTERNAL_PKG_DEPENDENCIES "\n" FIND_EXTERNAL_PKG_DEPENDENCIES)

# Copy all find modules
configure_file(cmake/SetupBlaze.cmake cmake/SetupBlaze.cmake COPYONLY)
configure_file(cmake/SetupBlazeTensor.cmake cmake/SetupBlazeTensor.cmake COPYONLY)
configure_file(cmake/SetupSleef.cmake cmake/SetupSleef.cmake COPYONLY)
configure_file(cmake/SetupBrigand.cmake cmake/SetupBrigand.cmake COPYONLY)
configure_file(cmake/SetupCxxOpts.cmake cmake/SetupCxxOpts.cmake COPYONLY)
configure_file(cmake/SetupYamlCpp.cmake cmake/SetupYamlCpp.cmake COPYONLY)
configure_file(cmake/SetupPybind11.cmake cmake/SetupPybind11.cmake COPYONLY)

# configure_file(cmake/FindBlaze.cmake cmake/FindBlaze.cmake COPYONLY)
# configure_file(cmake/FindBlazeTensor.cmake cmake/FindBlazeTensor.cmake COPYONLY)
configure_file(cmake/FindSleef.cmake cmake/FindSleef.cmake COPYONLY)
# configure_file(cmake/FindBrigand.cmake cmake/FindBrigand.cmake COPYONLY)
# configure_file(cmake/FindYamlCpp.cmake cmake/FindYamlCpp.cmake COPYONLY)

configure_file(cmake/GetEnvPath.cmake cmake/GetEnvPath.cmake COPYONLY)
configure_file(cmake/Logging.cmake cmake/Logging.cmake COPYONLY)
configure_file(cmake/ElasticaAddInterfaceLibraryHeaders.cmake cmake/ElasticaAddInterfaceLibraryHeaders.cmake COPYONLY)

set(FIND_EXTERNAL_PKG_DEPENDENCIES "")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(Logging)")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(ElasticaAddInterfaceLibraryHeaders)")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(SetupSleef)")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(SetupBrigand)")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(SetupBlaze)")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(SetupBlazeTensor)")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(SetupCxxOpts)")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(SetupYamlCpp)")
list(APPEND FIND_EXTERNAL_PKG_DEPENDENCIES "include(SetupPybind11)")
list(JOIN FIND_EXTERNAL_PKG_DEPENDENCIES "\n" FIND_EXTERNAL_PKG_DEPENDENCIES)

# find_dependency(Sleef MODULE REQUIRED)
# find_dependency(Blaze MODULE REQUIRED)
# find_dependency(BlazeTensor MODULE REQUIRED)
# find_dependency(Brigand MODULE REQUIRED)
# find_dependency(pybind11 MODULE REQUIRED)
# find_dependency(YamlCpp MODULE REQUIRED)
# find_dependency(cxxopts MODULE REQUIRED)

configure_package_config_file(
        "${CMAKE_SOURCE_DIR}/cmake/Install/${PROJECT_NAME}Config.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
        INSTALL_DESTINATION ${ELASTICA_INSTALL_CONFIGDIR}
        PATH_VARS CMAKE_INSTALL_INCLUDEDIR CMAKE_INSTALL_LIBDIR
)

unset(FIND_EXTERNAL_PKG_DEPENDENCIES)

write_basic_package_version_file(
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
        VERSION ${PROJECT_VERSION}
        COMPATIBILITY AnyNewerVersion
)


# Install to destination
install(
        FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Config.cmake"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}ConfigVersion.cmake"
        DESTINATION ${ELASTICA_INSTALL_CONFIGDIR}
        COMPONENT Development
)

# I don't want to use PROJECT_* because CMake reserves it
# So I use PKG, similar to libCCD
if (NOT MSVC)
    set(PKG_CFLAGS "-std=c++14")
endif ()

# needs a string variable here
list(JOIN PKG_EXTERNAL_DEPS " " PKG_EXTERNAL_DEPS)
configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/Install/${PROJECT_NAME}.pc.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}.pc"
        @ONLY
)
unset(PKG_EXTERNAL_DEPS)

install(FILES "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}.pc"
        DESTINATION "${CMAKE_INSTALL_LIBDIR}/pkgconfig"
        COMPONENT Development
        )

# Add uninstall target
configure_file(
        "${CMAKE_SOURCE_DIR}/cmake/Install/cmake_uninstall.cmake.in"
        "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake"
        IMMEDIATE @ONLY)
add_custom_target(uninstall
        "${CMAKE_COMMAND}" -P "${CMAKE_CURRENT_BINARY_DIR}/cmake/cmake_uninstall.cmake")

# get_property(
#         ELASTICA_LIBS
#         GLOBAL
#         PROPERTY
#         ELASTICA_LIBS_PROPERTY)

#foreach (ELASTICA_LIB ${ELASTICA_LIBS})
#    install(TARGETS ${ELASTICA_LIB}
#            EXPORT ${ELASTICA_LIB}Targets
#            ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#            # COMPONENT Development
#            LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#            # COMPONENT Development
#            RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#            # COMPONENT Runtime
#            INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#            )
#
#    install(EXPORT ${ELASTICA_LIB}Targets
#            FILE ${ELASTICA_LIB}Targets.cmake
#            NAMESPACE elastica::
#            DESTINATION ${ELASTICA_INSTALL_CONFIGDIR}
#            # COMPONENT Development
#            )
#endforeach (ELASTICA_LIB ${ELASTICA_LIBS})
#
#install(TARGETS ${PROJECT_NAME}
#        EXPORT ${PROJECT_NAME}Targets
#        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        # COMPONENT Development
#        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
#        # COMPONENT Development
#        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
#        # COMPONENT Runtime
#        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
#        )
#
#install(EXPORT ${PROJECT_NAME}Targets
#        FILE ${PROJECT_NAME}Targets.cmake
#        NAMESPACE elastica::
#        DESTINATION ${ELASTICA_INSTALL_CONFIGDIR}
#        # COMPONENT Development
#        )
#
## For custom files written during build files
#install(DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/elastica DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

# https://cmake.org/cmake/help/latest/guide/importing-exporting/index.html#exporting-targets-from-the-build-tree
get_target_property(ELASTICA_LIBS libs MANUALLY_ADDED_DEPENDENCIES)

foreach (ELASTICA_LIB ${ELASTICA_LIBS})
    # Begin exports of all targets built
    export(TARGETS ${ELASTICA_LIB}
            FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${ELASTICA_LIB}Targets.cmake"
            NAMESPACE elastica::
            )
endforeach (ELASTICA_LIB ${ELASTICA_LIBS})

export(TARGETS ElasticaFlags
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/ElasticaFlagsTargets.cmake"
        NAMESPACE elastica::
        )

# repurposed from https://stackoverflow.com/a/57100890
macro(get_all_linked_libraries iTarget iReturnValue)
    if (NOT TARGET ${iTarget})
        message(WARNING "${iTarget} is not a target")
    else ()
        get_target_property(linkedLibraries ${iTarget} INTERFACE_LINK_LIBRARIES)
        if (NOT "${linkedLibraries}" STREQUAL "linkedLibraries-NOTFOUND")
            foreach (linkedLibrary ${linkedLibraries})
                list(APPEND ${iReturnValue} ${linkedLibrary})
                get_all_linked_libraries(${linkedLibrary} ${iReturnValue})
            endforeach ()
        endif ()
    endif ()
endmacro()

get_all_linked_libraries(ElasticaFlags DEPS_ELASTICA_FLAGS)

foreach (DEP_ELASTICA_FLAG ${DEPS_ELASTICA_FLAGS})
    # Begin exports of all targets built
    export(TARGETS ${DEP_ELASTICA_FLAG}
            FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${DEP_ELASTICA_FLAG}Targets.cmake"
            NAMESPACE elastica::
            )
endforeach (DEP_ELASTICA_FLAG ${DEPS_ELASTICA_FLAGS})
unset(DEPS_ELASTICA_FLAGS)

# Begin exports of all targets built
export(TARGETS ${PROJECT_NAME}
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/${PROJECT_NAME}Targets.cmake"
        NAMESPACE elastica::
        )

export(PACKAGE ${PROJECT_NAME})

unset(ELASTICA_LIBS)
