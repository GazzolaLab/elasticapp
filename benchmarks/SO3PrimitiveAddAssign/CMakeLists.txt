set(BENCHMARK "BenchmarkSO3PrimitiveAddAssign")
set(PROFILE "ProfileSO3PrimitiveAddAssign")

add_executable(${BENCHMARK} BenchmarkSO3PrimitiveAddAssign.cpp)
add_executable(${PROFILE} ProfileSO3PrimitiveAddAssign.cpp)

file(COPY
        "${CMAKE_CURRENT_SOURCE_DIR}/run_so3_profile.sh"
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