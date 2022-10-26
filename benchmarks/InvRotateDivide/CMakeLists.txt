set(BENCHMARK "BenchmarkInvRotateDivide")
set(PROFILE "ProfileInvRotateDivide")

add_executable(${BENCHMARK} BenchmarkInvRotateDivide.cpp)
add_executable(${PROFILE} ProfileInvRotateDivide.cpp)

file(COPY
        "${CMAKE_CURRENT_SOURCE_DIR}/run_inverse_rotate_profile.sh"
        DESTINATION "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}"
        FILE_PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
        )

foreach (EXEC ${BENCHMARK};${PROFILE})
    target_link_libraries(${EXEC}
            PUBLIC
            Utilities
            ErrorHandling
            ModuleSettings
            Blaze
            BlazeTensor
            Systems
            ElasticaFlags
            )

endforeach ()
