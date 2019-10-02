# Distributed under the MIT License. See LICENSE.txt for details.

log_info("Finding Blaze")

find_package(Blaze 3.2 REQUIRED)

elastica_include_directories("${BLAZE_INCLUDE_DIR}")
log_debug("BLAZE_INCLUDE_DIR: ${BLAZE_INCLUDE_DIR}")
log_debug("BLAZE_VERSION: ${BLAZE_VERSION}")

file(APPEND "${CMAKE_BINARY_DIR}/LibraryVersions.txt"
     "Blaze Version:  ${BLAZE_VERSION}\n")
