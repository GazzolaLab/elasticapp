# Distributed under the MIT License.
# See LICENSE.txt for details.
# Obtained with thanks from https://github.com/sxs-collaboration/spectre

option(USE_CCACHE "Use CCache if available to speed up builds" ON)
if (USE_CCACHE)
    find_program(CCACHE_FOUND ccache)
    if (CCACHE_FOUND)
        set(CCACHE_COMMAND "ccache")
        # CCache offers no benefit for linking
        set(CMAKE_CXX_COMPILER_LAUNCHER ${CCACHE_COMMAND})
        log_info("Using ccache for compilation. It is invoked as: ${CCACHE_COMMAND}")
    else ()
        log_info("Could not find ccache")
    endif (CCACHE_FOUND)
endif (USE_CCACHE)
