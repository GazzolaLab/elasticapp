# Distributed under the MIT License.
# See LICENSE.txt for details.

if(NOT CXXOPTS_ROOT)
  set(CXXOPTS_ROOT "")
  set(CXXOPTS_ROOT $ENV{cxxopts_ROOT})
endif()

find_path(
  cxxopts_INCLUDE_DIR
  PATH_SUFFIXES include cxxopts
  NAMES cxxopts.hpp
  HINTS ${CMAKE_SOURCE_DIR}/external ${CXXOPTS_ROOT}
  DOC "cxxopts include directory. Used cxxopts_ROOT to set a search dir."
)

function(extract_defined_value FILE DEFINE_NAME OUT_VAR)
  file(READ "${FILE}" _contents)
  string(REGEX MATCH "#define[ \t]+${DEFINE_NAME}[ \t]+([0-9]+)"
    _match "${_contents}")

  if(NOT CMAKE_MATCH_1)
    message(FATAL_ERROR "Could not find ${DEFINE_NAME} in ${FILE}")
  endif()

  set(${OUT_VAR} "${CMAKE_MATCH_1}" PARENT_SCOPE)
endfunction()

if(cxxopts_INCLUDE_DIR)
  extract_defined_value(
    "${cxxopts_INCLUDE_DIR}/cxxopts.hpp"
    CXXOPTS__VERSION_MAJOR
    cxxopts_VERSION_MAJOR
  )
  extract_defined_value(
    "${cxxopts_INCLUDE_DIR}/cxxopts.hpp"
    CXXOPTS__VERSION_MINOR
    cxxopts_VERSION_MINOR
  )
  extract_defined_value(
    "${cxxopts_INCLUDE_DIR}/cxxopts.hpp"
    CXXOPTS__VERSION_PATCH
    cxxopts_VERSION_PATCH
  )
  set(cxxopts_VERSION "${cxxopts_VERSION_MAJOR}.${cxxopts_VERSION_MINOR}.${cxxopts_VERSION_PATCH}")
endif()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(cxxopts
  cxxopts_INCLUDE_DIR
  cxxopts_VERSION
)

mark_as_advanced(cxxopts_INCLUDE_DIR)
mark_as_advanced(cxxopts_VERSION)