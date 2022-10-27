set(BENCHMARK "BenchmarkMatVecOp")
set(PROFILE "ProfileMatVecOp")

add_executable(${BENCHMARK} ${BENCHMARK}.cpp)
add_executable(${PROFILE} ${PROFILE}.cpp)

file(COPY
        "${CMAKE_CURRENT_SOURCE_DIR}/run_mat_vec_op_profile.sh"
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
