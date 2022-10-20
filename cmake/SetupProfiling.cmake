# Distributed under the MIT License. See LICENSE.txt for details.

option(ELASTICA_ENABLE_PROFILING "Enable/Disable intrusive code profiling." ON)
option(KEEP_FRAME_POINTER "Add keep frame pointer for profiling" OFF)

add_library(_Profiling_EnableProfiling INTERFACE)
add_library(_Profiling_KeepFramePointer INTERFACE)

if (ELASTICA_ENABLE_PROFILING)
    set_property(
            TARGET _Profiling_EnableProfiling
            APPEND PROPERTY
            INTERFACE_COMPILE_DEFINITIONS
            $<$<COMPILE_LANGUAGE:CXX>:ELASTICA_PROFILING_MODE>
    )
endif ()

if (KEEP_FRAME_POINTER)
    set_property(
            TARGET _Profiling_KeepFramePointer
            APPEND PROPERTY
            INTERFACE_COMPILE_OPTIONS
            $<$<COMPILE_LANGUAGE:CXX>:-fno-omit-frame-pointer>
            $<$<COMPILE_LANGUAGE:CXX>:-mno-omit-leaf-frame-pointer>
    )
endif ()


target_link_libraries(
        ElasticaFlags
        INTERFACE
        _Profiling_EnableProfiling
        _Profiling_KeepFramePointer
)