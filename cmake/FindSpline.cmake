# Distributed under the MIT License.
# See LICENSE.txt for details.

if(NOT SPLINE_ROOT)
    # Need to set to empty to avoid warnings with --warn-uninitialized
    set(SPLINE_ROOT "")
    set(SPLINE_ROOT $ENV{SPLINE_ROOT})
endif()

find_path(
        SPLINE_INCLUDE_DIR
         PATH_SUFFIXES include
        NAMES spline.h
        HINTS ${SPLINE_ROOT}
        DOC "Spline include directory. Used SPLINE_ROOT to set a search dir."
)

set(SPLINE_INCLUDE_DIRS ${SPLINE_INCLUDE_DIR})

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(
        Spline
        DEFAULT_MSG SPLINE_INCLUDE_DIR SPLINE_INCLUDE_DIRS
)
mark_as_advanced(SPLINE_INCLUDE_DIR SPLINE_INCLUDE_DIRS)
