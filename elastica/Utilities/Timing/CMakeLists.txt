set(LIBRARY Utilities)

elastica_target_headers(
        ${LIBRARY}
        INCLUDE_DIRECTORY ${CMAKE_SOURCE_DIR}
        HEADERS
        CpuPolicy.h
        CpuTimer.h
        WcPolicy.h
        WcTimer.h
        Timing.h
        Timer.h
        Tags.hpp
)
