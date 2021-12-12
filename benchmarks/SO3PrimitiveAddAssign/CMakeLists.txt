set(BENCHMARK "BenchmarkSO3PrimitiveAddAssign")
set(PROFILE "ProfileSO3PrimitiveAddAssign")

add_executable(${BENCHMARK} BenchmarkSO3PrimitiveAddAssign.cpp)
add_executable(${PROFILE} ProfileSO3PrimitiveAddAssign.cpp)

foreach (EXEC ${BENCHMARK};${PROFILE})
    target_link_libraries(${EXEC}
            PUBLIC
            Utilities
            ErrorHandling
            Blaze
            BlazeTensor
            Systems
            ElasticaFlags
            # Sleef
            )

endforeach ()