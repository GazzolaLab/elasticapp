# Distributed under the MIT License. See LICENSE.txt for details.

option(ELASTICA_ENABLE_PROFILING "Enable/Disable intrusive code profiling." ON)

add_library(_EnableProfiling INTERFACE)

if (ELASTICA_ENABLE_PROFILING)
    set_property(
            TARGET _EnableProfiling
            APPEND PROPERTY
            INTERFACE_COMPILE_DEFINITIONS
            $<$<COMPILE_LANGUAGE:CXX>:ELASTICA_PROFILING_MODE>
    )
endif ()

target_link_libraries(
        ElasticaFlags
        INTERFACE
        _EnableProfiling
)

#if (ELASTICA_PROFILING_MODE)
#    set(ELASTICA_PROFILING_MODE 1)
#else ()
#    set(ELASTICA_PROFILING_MODE 0)
#endif ()
#configure_file("${CMAKE_CURRENT_LIST_DIR}/elastica/Configuration/Profiling.hpp.in"
#        "${CMAKE_CURRENT_BINARY_DIR}/elastica/Configuration/Profiling.hpp")

