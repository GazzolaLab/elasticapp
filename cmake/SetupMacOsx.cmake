# Distributed under the MIT License.
# See LICENSE.txt for details.
# Obtained with thanks from https://github.com/sxs-collaboration/spectre

# https://stackoverflow.com/a/19819591
if (APPLE)
    set(ELASTICA_MACOSX_MIN "10.9")
    if (DEFINED MACOSX_MIN)
        set(ELASTICA_MACOSX_MIN "${MACOSX_MIN}")
    endif ()
    set(CMAKE_EXE_LINKER_FLAGS
            "${CMAKE_EXE_LINKER_FLAGS} -mmacosx-version-min=${ELASTICA_MACOSX_MIN}")
    log_info("Minimum macOS version: ${ELASTICA_MACOSX_MIN}")
endif ()
