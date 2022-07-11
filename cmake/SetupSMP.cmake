# Distributed under the MIT License. See LICENSE.txt for details.

option(ELASTICA_SMP "Enable/Disable the shared-memory parallelization." OFF)

add_library(ParallelUseSMP INTERFACE)
add_library(ParallelUseTBB INTERFACE)

if (ELASTICA_SMP)
    set_property(
            TARGET ParallelUseSMP
            APPEND PROPERTY
            INTERFACE_COMPILE_DEFINITIONS
            $<$<COMPILE_LANGUAGE:CXX>:ELASTICA_USE_SHARED_MEMORY_PARALLELIZATION>
    )
    set(ELASTICA_SMP_THREADS "TBB" CACHE STRING "Specify which thread library is used for shared-memory parallelization.")
    set_property(CACHE ELASTICA_SMP_THREADS PROPERTY STRINGS "TBB;C++11")
else ()
    unset(ELASTICA_SMP_THREADS CACHE)
endif ()

if (ELASTICA_SMP_THREADS STREQUAL "TBB")
    include(SetupTBB)
    if (TBB_FOUND)
        add_library(ParallelBackend ALIAS TBB)
        set_property(
                TARGET ParallelUseTBB
                APPEND PROPERTY
                INTERFACE_COMPILE_DEFINITIONS
                $<$<COMPILE_LANGUAGE:CXX>:ELASTICA_USE_TBB>
        )
    else ()
        message(WARNING "TBB not found. Elastica will running on a single thread. Try C++11.")
    endif ()
else ()
    add_library(ParallelBackend IMPORTED INTERFACE)
endif ()

target_link_libraries(
        ElasticaFlags
        INTERFACE
        ParallelUseSMP
        ParallelUseTBB
)
