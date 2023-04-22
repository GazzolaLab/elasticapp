# Distributed under the MIT License. See LICENSE.txt for details.

log_info("Finding HighFive")

include(SetupHDF5)
find_package(HighFive)

if (HighFive_FOUND)
 log_debug("HighFive include: ${HIGHFIVE_INCLUDE_DIRS}")
 log_debug("HighFive version: ${HIGHFIVE_VERSION}")
 
 file(APPEND "${CMAKE_BINARY_DIR}/BuildInformation.txt"
         "HighFive Version:  ${HighFive_VERSION}\n")
 
 if (NOT TARGET HighFive)
     #    message(FATAL_ERROR "HighFive not found")
     add_library(HighFive INTERFACE IMPORTED)
     set_property(TARGET HighFive PROPERTY
             INTERFACE_INCLUDE_DIRECTORIES ${HIGHFIVE_INCLUDE_DIR})
 endif ()
 
 target_link_libraries(HighFive INTERFACE hdf5::hdf5)
 
 add_interface_lib_headers(
         TARGET HighFive
         HEADERS
         highfive/H5File.hpp
         highfive/H5Group.hpp
 )
 
 set_property(
        GLOBAL APPEND PROPERTY ELASTICA_THIRD_PARTY_LIBS
        HighFive
 )
else()
	message(STATUS "HighFive library not found")
endif()

