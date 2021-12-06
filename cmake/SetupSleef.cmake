log_info("Finding Sleef")

find_package(Sleef REQUIRED QUIET)

file(APPEND "${CMAKE_BINARY_DIR}/LibraryVersions.txt"
        "Sleef Version:  ${SLEEF_VERSION}\n")

add_library(Sleef INTERFACE IMPORTED)
set_property(TARGET Sleef PROPERTY
        INTERFACE_INCLUDE_DIRECTORIES ${SLEEF_INCLUDE_DIRS})
set_property(TARGET Sleef
        APPEND PROPERTY INTERFACE_LINK_LIBRARIES ${SLEEF_LIBRARIES})

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        Sleef
)

log_debug("SLEEF_INCLUDE_DIRS: ${SLEEF_INCLUDE_DIRS}")
log_debug("SLEEF_LIBRARIES: ${SLEEF_LIBRARIES}")
log_debug("SLEEF_VERSION: ${SLEEF_VERSION}")


