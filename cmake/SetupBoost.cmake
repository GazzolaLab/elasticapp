# Distributed under the MIT License. See LICENSE.txt for details.
# Obtained with thanks from https://github.com/sxs-collaboration/spectre

log_info("Finding Boost")

find_package(Boost 1.72.0)

log_debug("Boost include: ${Boost_INCLUDE_DIRS}")
log_debug("Boost libraries: ${Boost_LIBRARIES}")

file(APPEND
        "${CMAKE_BINARY_DIR}/LibraryVersions.txt"
        "Boost Version:  ${Boost_MAJOR_VERSION}.${Boost_MINOR_VERSION}.${Boost_SUBMINOR_VERSION}\n"
        )

# Boost organizes targets as:
# - Boost::boost is the header-only parts of Boost
# - Boost::COMPONENT are the components that need linking, e.g. program_options
#add_interface_lib_headers(
#        TARGET Boost::boost
#        HEADERS
#        boost/core/demangle.hpp
#)

#set_property(TARGET Brigand PROPERTY
#        INTERFACE_INCLUDE_DIRECTORIES ${BRIGAND_INCLUDE_DIR})

#set_property(
#        TARGET Boost::boost
#        APPEND PROPERTY PUBLIC_HEADER
#        boost/core/demangle.hpp
#)
#

set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        Boost::boost
)
