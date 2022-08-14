# Distributed under the MIT License.
# See LICENSE.txt for details.

# Find HighFive: https://github.com/BlueBrain/HighFive
# If not in one of the default paths specify -D HIGHFIVE_ROOT=/path/to/HighFive
# to search there as well.
# Alternatively you can set HighFive_DIR as well

include(CheckCXXSourceRuns)
include(GetEnvPath)

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(HIGHFIVE_ROOT)

if (NOT HIGHFIVE_ROOT)
    # Need to set to empty to avoid warnings with --warn-uninitialized
    set(HIGHFIVE_ROOT "")
endif ()

# find the HighFive include directory
find_path(HIGHFIVE_INCLUDE_DIR
        PATH_SUFFIXES include
        NAMES highfive/H5File.hpp
        HINTS ${HIGHFIVE_ROOT} ${ENV_HIGHFIVE_ROOT}
        DOC "HIGHFIVE include directory. Use HIGHFIVE_ROOT to set a search dir.")

set(HIGHFIVE_INCLUDE_DIRS ${HIGHFIVE_INCLUDE_DIR})
set(HIGHFIVE_VERSION "")

# Extract version info from header
if (EXISTS "${HIGHFIVE_INCLUDE_DIRS}/highfive/H5Version.hpp")
    file(READ "${HIGHFIVE_INCLUDE_DIRS}/highfive/H5Version.hpp"
            HIGHFIVE_FIND_HEADER_CONTENTS)

    string(REGEX MATCH
            "#define HIGHFIVE_VERSION_MAJOR [0-9]+"
            HIGHFIVE_VERSION_MAJOR
            "${HIGHFIVE_FIND_HEADER_CONTENTS}")
    string(REPLACE "#define HIGHFIVE_VERSION_MAJOR "
            ""
            HIGHFIVE_VERSION_MAJOR
            ${HIGHFIVE_VERSION_MAJOR})

    string(REGEX MATCH
            "#define HIGHFIVE_VERSION_MINOR [0-9]+"
            HIGHFIVE_VERSION_MINOR
            "${HIGHFIVE_FIND_HEADER_CONTENTS}")
    string(REPLACE "#define HIGHFIVE_VERSION_MINOR "
            ""
            HIGHFIVE_VERSION_MINOR
            ${HIGHFIVE_VERSION_MINOR})

    string(REGEX MATCH
            "#define HIGHFIVE_VERSION_PATCH [0-9]+"
            HIGHFIVE_VERSION_PATCH
            "${HIGHFIVE_FIND_HEADER_CONTENTS}")
    string(REPLACE "#define HIGHFIVE_VERSION_PATCH "
            ""
            HIGHFIVE_VERSION_PATCH
            ${HIGHFIVE_VERSION_PATCH})

    set(HIGHFIVE_VERSION "${HIGHFIVE_VERSION_MAJOR}.${HIGHFIVE_VERSION_MINOR}.${HIGHFIVE_VERSION_PATCH}")
else ()
    message(WARNING "Failed to find file "
            "'${HIGHFIVE_INCLUDE_DIRS}/highfive/H5Version.hpp' "
            "while detecting the HighFive version.")
endif ()

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(HighFive
        FOUND_VAR
        HIGHFIVE_FOUND
        REQUIRED_VARS
        HIGHFIVE_INCLUDE_DIRS
        VERSION_VAR
        HIGHFIVE_VERSION)

mark_as_advanced(HIGHFIVE_INCLUDE_DIR HIGHFIVE_INCLUDE_DIRS
        HIGHFIVE_VERSION
        HIGHFIVE_VERSION_MAJOR
        HIGHFIVE_VERSION_MINOR
        HIGHFIVE_VERSION_PATCH
        )
