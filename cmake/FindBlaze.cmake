# Distributed under the MIT License. See LICENSE.txt for details.
# Obtained with thanks from https://github.com/sxs-collaboration/spectre

# ===============================================
# See if we have env vars to help us find tbb
# ===============================================
macro(getenv_path VAR)
    set(ENV_${VAR} $ENV{${VAR}})
    # replace won't work if var is blank
    if(ENV_${VAR})
        string(REGEX
               REPLACE "\\\\"
                       "/"
                       ENV_${VAR}
                       ${ENV_${VAR}})
    endif()
endmacro()

# Get path, convert backslashes as ${ENV_${var}}
getenv_path(BLAZE_ROOT)

find_path(BLAZE_INCLUDE_DIR
          PATH_SUFFIXES include
          NAMES blaze/Blaze.h
          HINTS ${BLAZE_ROOT} ${ENV_BLAZE_ROOT}
          DOC "Blaze include directory. Used BLAZE_ROOT to set a search dir.")

set(BLAZE_INCLUDE_DIRS ${BLAZE_INCLUDE_DIR})

# Extract version info from header
file(READ "${BLAZE_INCLUDE_DIRS}/blaze/system/Version.h"
     BLAZE_FIND_HEADER_CONTENTS)

string(REGEX MATCH
             "#define BLAZE_MAJOR_VERSION [0-9]+"
             BLAZE_MAJOR_VERSION
             "${BLAZE_FIND_HEADER_CONTENTS}")
string(REPLACE "#define BLAZE_MAJOR_VERSION "
               ""
               BLAZE_MAJOR_VERSION
               ${BLAZE_MAJOR_VERSION})

string(REGEX MATCH
             "#define BLAZE_MINOR_VERSION [0-9]+"
             BLAZE_MINOR_VERSION
             "${BLAZE_FIND_HEADER_CONTENTS}")
string(REPLACE "#define BLAZE_MINOR_VERSION "
               ""
               BLAZE_MINOR_VERSION
               ${BLAZE_MINOR_VERSION})

set(BLAZE_VERSION "${BLAZE_MAJOR_VERSION}.${BLAZE_MINOR_VERSION}")

set(Blaze_VERSION "${BLAZE_MAJOR_VERSION}.${BLAZE_MINOR_VERSION}")

include(FindPackageHandleStandardArgs)
find_package_handle_standard_args(Blaze
                                  FOUND_VAR
                                  BLAZE_FOUND
                                  REQUIRED_VARS
                                  BLAZE_INCLUDE_DIR
                                  BLAZE_INCLUDE_DIRS
                                  VERSION_VAR
                                  BLAZE_VERSION)
mark_as_advanced(BLAZE_INCLUDE_DIR
                 BLAZE_INCLUDE_DIRS
                 BLAZE_VERSION
                 BLAZE_MAJOR_VERSION
                 BLAZE_MINOR_VERSION)
